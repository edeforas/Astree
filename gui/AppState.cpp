// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include "App.h"

#include "DeviceIo.h"
#include "DeviceIoZmx.h"
#include "GlassCatalogIo.h"
#include "GlassManager.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <sstream>
#include <system_error>

namespace astree_imgui_app
{
std::string optimizerKey(int row, const char* field)
{
    return "optimizer." + std::to_string(row) + "." + field;
}

std::string lowerCase(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
    return value;
}

void markChanged(App& app, const char* message)
{
    app.dirty = true;
    app.status = message;
}

void loadOptimizerSettings(App& app)
{
    for (OptimizerRow& row : app.optimizerRows)
        row = OptimizerRow();
    app.optimizerRowCount = kInitialOptimizerParameterRows;
    app.optimizerMethod = 0;
    app.meritFunction = 0;
    app.minimumVignetting = 100.0;

    std::string value;
    const bool hasSavedRowCount =
        app.device.get_parameter("optimizer.rowCount", value);
    if (hasSavedRowCount)
        app.optimizerRowCount = std::clamp(std::atoi(value.c_str()),
            kInitialOptimizerParameterRows, kMaxOptimizerParameters);

    for (int i = 0; i < kMaxOptimizerParameters; ++i)
    {
        OptimizerRow& row = app.optimizerRows[static_cast<std::size_t>(i)];
        bool configured = false;
        if (app.device.get_parameter(optimizerKey(i, "checked"), value))
        {
            row.enabled = value == "1";
            configured |= row.enabled;
        }
        if (app.device.get_parameter(optimizerKey(i, "surface"), value))
        {
            row.surface = std::max(1, std::atoi(value.c_str()));
            configured |= row.surface != 1;
        }
        if (app.device.get_parameter(optimizerKey(i, "parameter"), value))
        {
            configured |= !value.empty();
            for (int p = 1; p < 9; ++p)
                if (value == kParameterNames[p])
                    row.parameter = p;
        }
        std::string minValue;
        std::string maxValue;
        if (app.device.get_parameter(optimizerKey(i, "min"), minValue) &&
            app.device.get_parameter(optimizerKey(i, "max"), maxValue) &&
            !minValue.empty() && !maxValue.empty())
        {
            row.minimum = std::strtod(minValue.c_str(), nullptr);
            row.maximum = std::strtod(maxValue.c_str(), nullptr);
            row.automaticBounds = false;
            configured = true;
            std::snprintf(row.minimumText, sizeof(row.minimumText), "%.6g", row.minimum);
            std::snprintf(row.maximumText, sizeof(row.maximumText), "%.6g", row.maximum);
        }
        if (!hasSavedRowCount && configured)
            app.optimizerRowCount = std::max(app.optimizerRowCount, i + 1);
    }

    if (app.device.get_parameter("optimizer.merit", value))
    {
        if (value == "MostlyCenter") app.meritFunction = 1;
        else if (value == "FullFrameMean") app.meritFunction = 2;
        else if (value == "FullFrameMaxError") app.meritFunction = 3;
        else app.meritFunction = 0;
    }
    if (app.device.get_parameter("optimizer.method", value))
    {
        if (value == "MonteCarlo") app.optimizerMethod = 1;
        else if (value == "Hypercube") app.optimizerMethod = 2;
        else app.optimizerMethod = 0;
    }
    if (app.device.get_parameter("optimizer.minVignetting", value))
        app.minimumVignetting = std::strtod(value.c_str(), nullptr);
}

void saveOptimizerSettings(App& app)
{
    app.device.set_parameter("optimizer.rowCount", std::to_string(app.optimizerRowCount));
    for (int i = 0; i < kMaxOptimizerParameters; ++i)
    {
        const OptimizerRow& row = app.optimizerRows[static_cast<std::size_t>(i)];
        app.device.set_parameter(optimizerKey(i, "checked"), row.enabled ? "1" : "0");
        app.device.set_parameter(optimizerKey(i, "surface"), std::to_string(row.surface));
        app.device.set_parameter(optimizerKey(i, "parameter"), kParameterNames[row.parameter]);
        if (!row.automaticBounds)
        {
            std::ostringstream minimum;
            std::ostringstream maximum;
            minimum << std::setprecision(17) << row.minimum;
            maximum << std::setprecision(17) << row.maximum;
            app.device.set_parameter(optimizerKey(i, "min"), minimum.str());
            app.device.set_parameter(optimizerKey(i, "max"), maximum.str());
        }
        else
        {
            app.device.set_parameter(optimizerKey(i, "min"), "");
            app.device.set_parameter(optimizerKey(i, "max"), "");
        }
    }
    static const char* const meritNames[] = {
        "CenterOnly", "MostlyCenter", "FullFrameMean", "FullFrameMaxError"
    };
    static const char* const methodNames[] = { "NelderMead", "MonteCarlo", "Hypercube" };
    app.device.set_parameter("optimizer.merit", meritNames[app.meritFunction]);
    app.device.set_parameter("optimizer.method", methodNames[app.optimizerMethod]);
    app.device.set_parameter("optimizer.minVignetting", std::to_string(app.minimumVignetting));
}

void fitOpticalLayout(App& app)
{
    app.layoutZoom = 1.0;
    app.layoutPan = ImVec2(0.0f, 0.0f);
}

void syncUiFromDevice(App& app)
{
    app.displayAspheric = app.device.has_aspheric();
    app.displayInnerDiameter = app.device.has_inner_diameter();
    app.displayComments = app.device.has_comment();
    app.displayTilt = app.device.has_tilt();
    app.fov = app.device.half_field_of_view();
    app.intermediateAngles = std::max(1, app.device.nb_intermediate_angles());
    app.relativeConvention = app.device.relative_convention();
    app.autofocus = app.device.get_autofocus();
    app.autoCurvature = app.device.get_image_autocurvature();
    const std::string lightColors = app.device.light_colors();
    for (std::size_t i = 0; i < app.colors.size(); ++i)
        app.colors[i] = lightColors.find(std::string(kColorNames[i]) + ".") != std::string::npos;
    app.offAxisPercent = 0.0;
    app.device.set_parameter("showLightOffAxis", app.offAxisPercent);
    app.selectedImageQualityRow = 0;
    const ImageQuality quality = app.device.get_image_quality();
    for (int row = 0; row < quality.nb_angles(); ++row)
        if (quality.vdAngles[static_cast<std::size_t>(row)] == 0.0)
        {
            app.selectedImageQualityRow = row;
            break;
        }
    app.dirty = false;
    std::fill(std::begin(app.noteBuffer), std::end(app.noteBuffer), '\0');
    const std::string note = app.device.note();
    std::copy_n(note.c_str(), std::min(note.size(), sizeof(app.noteBuffer) - 1), app.noteBuffer);
    loadOptimizerSettings(app);
}

void loadGlassCatalogs(App& app)
{
    std::error_code error;
    const fs::path catalogPath(ASTREE_GLASS_DIR);
    if (!fs::exists(catalogPath, error))
    {
        app.status = "Glass catalog directory is missing: " + catalogPath.string();
        return;
    }
    for (const auto& entry : fs::directory_iterator(catalogPath, error))
    {
        if (error)
        {
            app.status = "Unable to enumerate glass catalogs: " + error.message();
            return;
        }
        if (entry.is_regular_file() && lowerCase(entry.path().extension().string()) == ".agf" &&
            !GlassCatalogIO::load(entry.path().string(), GlassManager::singleton()))
        {
            app.status = "Unable to load glass catalog: " + entry.path().filename().string();
            return;
        }
    }
}

void rememberRecentFile(App& app, const fs::path& path, bool zmx)
{
    std::error_code error;
    fs::path absolutePath = fs::absolute(path, error);
    if (error)
        absolutePath = path;
    const std::string storedPath = absolutePath.lexically_normal().u8string();
#ifdef _WIN32
    const std::string comparisonPath = lowerCase(storedPath);
#else
    const std::string& comparisonPath = storedPath;
#endif
    app.recentFiles.erase(std::remove_if(app.recentFiles.begin(), app.recentFiles.end(),
        [&](const RecentFile& recent) {
#ifdef _WIN32
            return recent.zmx == zmx && lowerCase(recent.path) == comparisonPath;
#else
            return recent.zmx == zmx && recent.path == comparisonPath;
#endif
        }), app.recentFiles.end());
    app.recentFiles.insert(app.recentFiles.begin(), { storedPath, zmx });
    if (app.recentFiles.size() > kMaxRecentFiles)
        app.recentFiles.resize(kMaxRecentFiles);
}

bool loadDesign(App& app, const fs::path& path, bool zmx)
{
    std::unique_ptr<OpticalDevice> loaded(
        zmx ? DeviceIoZmx::import(path.string()) : DeviceIo::load(path.string()));
    if (!loaded)
    {
        app.status = "Could not load file: " + path.string();
        return false;
    }
    app.device = *loaded;
    app.fileName = zmx ? std::string() : path.string();
    syncUiFromDevice(app);
    fitOpticalLayout(app);
    rememberRecentFile(app, path, zmx);
    app.status = "Loaded " + path.filename().string();
    return true;
}

bool saveDesign(App& app, const fs::path& path)
{
    saveOptimizerSettings(app);
    if (!DeviceIo::save(path.string(), &app.device))
    {
        app.status = "Could not save file: " + path.string();
        return false;
    }
    app.fileName = path.string();
    app.dirty = false;
    app.status = "Saved " + path.filename().string();
    return true;
}

void executePendingAction(App& app)
{
    const PendingAction action = app.pendingAction;
    app.pendingAction = PendingAction::None;
    app.confirmPendingAction = false;
    if (action == PendingAction::NewDesign)
    {
        app.device = OpticalDevice();
        app.fileName.clear();
        syncUiFromDevice(app);
        app.status = "New design";
    }
    else if (action == PendingAction::LoadDesign || action == PendingAction::ImportZmx)
    {
        loadDesign(app, app.pendingPath, action == PendingAction::ImportZmx);
        app.pendingPath.clear();
    }
    else if (action == PendingAction::Quit)
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
}

void requestAction(App& app, PendingAction action, const fs::path& path)
{
    app.pendingAction = action;
    app.pendingPath = path.string();
    if (app.dirty)
        app.confirmPendingAction = true;
    else
        executePendingAction(app);
}

bool acceptFileSelection(App& app, fs::path target)
{
    const std::string requiredExtension = app.importZmx ? ".zmx" : ".astree";
    if (lowerCase(target.extension().string()) != requiredExtension)
        target += requiredExtension;
    if (app.saveAs)
    {
        if (!saveDesign(app, target))
            return false;
        if (app.pendingAction != PendingAction::None)
            executePendingAction(app);
        return true;
    }
    requestAction(app, app.importZmx ? PendingAction::ImportZmx : PendingAction::LoadDesign,
        target);
    return true;
}
}
