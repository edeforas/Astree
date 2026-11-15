// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include "DeviceOptimizerAmoeba.h"
#include "DeviceOptimizerHypercube.h"
#include "DeviceOptimizerRandom.h"
#include "DeviceRevert.h"
#include "DeviceScaling.h"
#include "Glass.h"
#include "GlassManager.h"
#include "ImageQuality.h"
#include "Light.h"
#include "Photon.h"
#include <GLFW/glfw3.h>
#include "imgui_internal.h"
#include <algorithm>
#include <cctype>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace astree_imgui_app
{
namespace
{
OptimizerCompletion runOptimizer(OpticalDevice device, int method, int meritFunction,
    double minimumVignetting, const std::array<OptimizerRow, kMaxOptimizerParameters>& rows,
    int rowCount)
{
    std::unique_ptr<DeviceOptimizer> optimizer;
    if (method == 0) optimizer = std::make_unique<DeviceOptimizerAmoeba>();
    else if (method == 1) optimizer = std::make_unique<DeviceOptimizerRandom>();
    else optimizer = std::make_unique<DeviceOptimizerHypercube>();
    optimizer->set_device(&device);
    optimizer->set_min_vignetting(minimumVignetting);
    static const OptimizerMeritFunction merits[] = {
        eCenterOnly, eMostlyCenter, eFullFrameMean, eFullFrameMaxError
    };
    optimizer->set_merit_function(merits[meritFunction]);

    const int surfaceCount = device.nb_surface();
    for (int i = 0; i < rowCount; ++i)
    {
        const OptimizerRow& row = rows[static_cast<std::size_t>(i)];
        if (!row.enabled || row.parameter <= 0 ||
            row.parameter >= static_cast<int>(std::size(kParameterNames)) ||
            row.surface < 1 || row.surface > surfaceCount)
            continue;
        if (row.automaticBounds)
            optimizer->add_parameter(row.surface - 1, kParameterNames[row.parameter]);
        else
            optimizer->add_parameter(row.surface - 1, kParameterNames[row.parameter],
                row.minimum, row.maximum);
    }

    const OptimizerResult result = optimizer->optimize();
    return { result, std::move(device) };
}

void pollOptimizer(App& app)
{
    if (!app.optimizerRunning ||
        app.optimizerTask.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        return;

    app.optimizerRunning = false;
    try
    {
        OptimizerCompletion completion = app.optimizerTask.get();
        switch (completion.result)
        {
        case eBetterSolutionFound:
            app.device = std::move(completion.device);
            markChanged(app, "Better solution found");
            break;
        case eNoBetterSolution:
            app.status = "No better solution found";
            break;
        case eSolutionOnEdge:
            app.device = std::move(completion.device);
            markChanged(app, "Solution on domain edge");
            break;
        case eNothingToOptimize:
            app.status = "Nothing to optimize";
            break;
        case eInvalidState:
            app.status = "Optimizer found an invalid state";
            break;
        }
    }
    catch (const std::exception& error)
    {
        app.status = std::string("Optimization failed: ") + error.what();
    }
}
}

void drawOptimizer(App& app)
{
    pollOptimizer(app);
    if (!ImGui::Begin("Optimizer"))
    {
        ImGui::End();
        return;
    }
    const char* methods[] = { "Nelder-Mead", "Monte Carlo", "Hypercube" };
    const char* criteria[] = {
        "Center only", "Mostly center", "Full-frame mean", "Full-frame max error"
    };
    bool settingsChanged = false;
    ImGui::BeginDisabled(app.optimizerRunning);
    if (ImGui::Button("Optimize"))
    {
        const int surfaceCount = app.device.nb_surface();
        const bool hasParameters = std::any_of(app.optimizerRows.begin(),
            app.optimizerRows.begin() + app.optimizerRowCount,
            [surfaceCount](const OptimizerRow& row)
        {
            return row.enabled && row.parameter > 0 &&
                row.parameter < static_cast<int>(std::size(kParameterNames)) &&
                row.surface >= 1 && row.surface <= surfaceCount;
        });
        if (!hasParameters)
        {
            app.status = "Nothing to optimize";
        }
        else
        {
            saveOptimizerSettings(app);
            app.dirty = true;
            app.status = "Optimizing...";
            try
            {
                app.optimizerTask = std::async(std::launch::async,
                    [device = app.device, method = app.optimizerMethod,
                     meritFunction = app.meritFunction,
                     minimumVignetting = app.minimumVignetting,
                     rows = app.optimizerRows,
                     rowCount = app.optimizerRowCount]() mutable
                    {
                        return runOptimizer(std::move(device), method, meritFunction,
                            minimumVignetting, rows, rowCount);
                    });
                app.optimizerRunning = true;
            }
            catch (const std::exception& error)
            {
                app.status = std::string("Could not start optimization: ") + error.what();
            }
        }
    }
    ImGui::EndDisabled();
    if (app.optimizerRunning)
        ImGui::OpenPopup("optimizer-progress");
    if (ImGui::BeginPopupModal("optimizer-progress", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (app.optimizerRunning)
            ImGui::TextUnformatted("Optimizing...");
        else
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::Separator();
    bool traceSettingsChanged = false;
    traceSettingsChanged |= ImGui::Checkbox("Autofocus", &app.autofocus);
    ImGui::SameLine();
    traceSettingsChanged |= ImGui::Checkbox(
        "Auto image curvature", &app.autoCurvature);
    if (traceSettingsChanged)
    {
        app.device.set_autofocus(app.autofocus);
        app.device.set_image_autocurvature(app.autoCurvature);
        app.device.get_image_quality();
        markChanged(app);
    }
    ImGui::Separator();
    const float labelWidths = ImGui::CalcTextSize("Method").x +
        ImGui::CalcTextSize("Criteria").x;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float comboWidth = std::max(1.0f,
        (ImGui::GetContentRegionAvail().x - labelWidths - spacing * 3.0f) * 0.5f);
    ImGui::TextUnformatted("Method");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);
    settingsChanged |= ImGui::Combo("##method", &app.optimizerMethod, methods, 3);
    ImGui::SameLine();
    ImGui::TextUnformatted("Criteria");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);
    settingsChanged |= ImGui::Combo("##criteria", &app.meritFunction, criteria, 4);
    ImGui::TextUnformatted("Minimum vignetting (%)");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    settingsChanged |= ImGui::InputDouble("##minimum-vignetting", &app.minimumVignetting);
    if (ImGui::BeginTable("optimizer-table", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        const int surfaceCount = app.device.nb_surface();
        ImGui::TableSetupColumn("Use", ImGuiTableColumnFlags_WidthFixed,
            ImGui::GetFrameHeight());
        ImGui::TableSetupColumn("Surface");
        ImGui::TableSetupColumn("Parameter");
        ImGui::TableSetupColumn("Min");
        ImGui::TableSetupColumn("Max");
        ImGui::TableHeadersRow();
        for (int i = 0; i < app.optimizerRowCount; ++i)
        {
            ImGui::PushID(i);
            OptimizerRow& row = app.optimizerRows[static_cast<std::size_t>(i)];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            settingsChanged |= ImGui::Checkbox("##enabled", &row.enabled);
            ImGui::TableSetColumnIndex(1);
            const int clampedSurface = std::clamp(row.surface, 1,
                std::max(1, surfaceCount));
            settingsChanged |= row.surface != clampedSurface;
            row.surface = clampedSurface;
            const std::string surfaceLabel = std::to_string(row.surface);
            ImGui::SetNextItemWidth(std::max(1.0f, ImGui::GetColumnWidth() -
                ImGui::GetStyle().CellPadding.x * 2.0f));
            ImGui::BeginDisabled(surfaceCount == 0);
            if (ImGui::BeginCombo("##surface", surfaceLabel.c_str()))
            {
                for (int surface = 1; surface <= surfaceCount; ++surface)
                {
                    const std::string surfaceOption = std::to_string(surface);
                    if (ImGui::Selectable(surfaceOption.c_str(), row.surface == surface))
                    {
                        row.surface = surface;
                        settingsChanged = true;
                    }
                    if (row.surface == surface)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::EndDisabled();
            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(std::max(1.0f, ImGui::GetColumnWidth() -
                ImGui::GetStyle().CellPadding.x * 2.0f));
            settingsChanged |= ImGui::Combo("##parameter", &row.parameter, kParameterNames, 9);
            ImGui::TableSetColumnIndex(3);
            ImGui::SetNextItemWidth(std::max(1.0f, ImGui::GetColumnWidth() -
                ImGui::GetStyle().CellPadding.x * 2.0f));
            if (ImGui::InputText("##minimum", row.minimumText, sizeof(row.minimumText)))
            {
                std::string text = lowerCase(row.minimumText);
                text.erase(text.begin(), std::find_if(text.begin(), text.end(),
                    [](unsigned char character) { return !std::isspace(character); }));
                text.erase(std::find_if(text.rbegin(), text.rend(),
                    [](unsigned char character) { return !std::isspace(character); }).base(), text.end());
                if (text == "auto")
                {
                    row.automaticBounds = true;
                    std::snprintf(row.minimumText, sizeof(row.minimumText), "Auto");
                    std::snprintf(row.maximumText, sizeof(row.maximumText), "Auto");
                }
                else
                {
                    char* end = nullptr;
                    const double value = std::strtod(row.minimumText, &end);
                    if (end != row.minimumText && *end == '\0')
                    {
                        row.minimum = value;
                        row.automaticBounds = false;
                        if (std::string(row.maximumText) == "Auto")
                            std::snprintf(row.maximumText, sizeof(row.maximumText), "%.6g", row.maximum);
                    }
                }
                settingsChanged = true;
            }
            ImGui::TableSetColumnIndex(4);
            ImGui::SetNextItemWidth(std::max(1.0f, ImGui::GetColumnWidth() -
                ImGui::GetStyle().CellPadding.x * 2.0f));
            if (ImGui::InputText("##maximum", row.maximumText, sizeof(row.maximumText)))
            {
                std::string text = lowerCase(row.maximumText);
                text.erase(text.begin(), std::find_if(text.begin(), text.end(),
                    [](unsigned char character) { return !std::isspace(character); }));
                text.erase(std::find_if(text.rbegin(), text.rend(),
                    [](unsigned char character) { return !std::isspace(character); }).base(), text.end());
                if (text == "auto")
                {
                    row.automaticBounds = true;
                    std::snprintf(row.minimumText, sizeof(row.minimumText), "Auto");
                    std::snprintf(row.maximumText, sizeof(row.maximumText), "Auto");
                }
                else
                {
                    char* end = nullptr;
                    const double value = std::strtod(row.maximumText, &end);
                    if (end != row.maximumText && *end == '\0')
                    {
                        row.maximum = value;
                        row.automaticBounds = false;
                        if (std::string(row.minimumText) == "Auto")
                            std::snprintf(row.minimumText, sizeof(row.minimumText), "%.6g", row.minimum);
                    }
                }
                settingsChanged = true;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::BeginDisabled(app.optimizerRowCount >= kMaxOptimizerParameters);
    if (ImGui::Button("Add parameter row") &&
        app.optimizerRowCount < kMaxOptimizerParameters)
    {
        ++app.optimizerRowCount;
        settingsChanged = true;
    }
    ImGui::EndDisabled();
    if (settingsChanged)
    {
        saveOptimizerSettings(app);
        markChanged(app);
    }
    ImGui::End();
}
}
