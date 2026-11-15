// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com

#include "App.h"

#include "GlassManager.h"
#include "ImageQuality.h"
#include "Light.h"
#include "Photon.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>
namespace astree_imgui_app
{
ImU32 toImColor(unsigned int rgb, unsigned int alpha = 255)
{
    return IM_COL32((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff, alpha);
}

bool intersectSegments(const ImVec2& a, const ImVec2& b,
    const ImVec2& c, const ImVec2& d, ImVec2& intersection)
{
    const double abX = b.x - a.x;
    const double abY = b.y - a.y;
    const double cdX = d.x - c.x;
    const double cdY = d.y - c.y;
    const double denominator = abX * cdY - abY * cdX;
    if (std::abs(denominator) < 1e-8)
        return false;

    const double acX = c.x - a.x;
    const double acY = c.y - a.y;
    const double alongAb = (acX * cdY - acY * cdX) / denominator;
    const double alongCd = (acX * abY - acY * abX) / denominator;
    if (alongAb <= 0.0 || alongAb >= 1.0 || alongCd <= 0.0 || alongCd >= 1.0)
        return false;

    intersection = ImVec2(
        static_cast<float>(a.x + alongAb * abX),
        static_cast<float>(a.y + alongAb * abY));
    return true;
}

void drawGlassSection(ImDrawList* draw, const ImVec2& currentStart,
    const ImVec2& currentEnd, const ImVec2& previousEnd,
    const ImVec2& previousStart, ImU32 fill)
{
    ImVec2 intersection;
    if (intersectSegments(currentStart, currentEnd, previousEnd, previousStart,
        intersection))
    {
        draw->AddTriangleFilled(currentStart, previousStart, intersection, fill);
        draw->AddTriangleFilled(currentEnd, previousEnd, intersection, fill);
        return;
    }

    const ImVec2 section[] = {
        currentStart, currentEnd, previousEnd, previousStart
    };
    draw->AddConcavePolyFilled(section, 4, fill);
}

void drawSurfaces(App& app)
{
    if (!ImGui::Begin("Surfaces"))
    {
        ImGui::End();
        return;
    }
    OpticalDevice& device = app.device;
    const bool hasTilt = device.has_tilt();
    const bool hasAspheric = device.has_aspheric();
    const bool hasInnerDiameter = device.has_inner_diameter();
    if (hasTilt)
        app.displayTilt = true;
    if (hasAspheric)
        app.displayAspheric = true;
    if (hasInnerDiameter)
        app.displayInnerDiameter = true;
    ImGui::TextUnformatted("Relative thickness");
    ImGui::SameLine();
    if (ImGui::Checkbox("##relative-thickness", &app.relativeConvention))
    {
        device.set_relative_convention(app.relativeConvention);
        markChanged(app);
    }
    ImGui::SameLine();
    ImGui::TextUnformatted("Aspheric");
    ImGui::SameLine();
    ImGui::BeginDisabled(hasAspheric);
    ImGui::Checkbox("##aspheric", &app.displayAspheric);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::TextUnformatted("InnerDiameter");
    ImGui::SameLine();
    ImGui::BeginDisabled(hasInnerDiameter);
    ImGui::Checkbox("##inner-diameter", &app.displayInnerDiameter);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::TextUnformatted("Comments");
    ImGui::SameLine();
    ImGui::Checkbox("##comments", &app.displayComments);
    ImGui::SameLine();
    ImGui::TextUnformatted("Tilt");
    ImGui::SameLine();
    ImGui::BeginDisabled(hasTilt);
    ImGui::Checkbox("##tilt", &app.displayTilt);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Add surface"))
    {
        app.selectedSurface = device.nb_surface();
        device.insert_surface(app.selectedSurface);
        markChanged(app);
    }
    ImGui::SameLine();
    const bool hasSelectedSurface =
        app.selectedSurface >= 0 && app.selectedSurface < device.nb_surface();
    ImGui::BeginDisabled(!hasSelectedSurface);
    if (ImGui::Button("Delete surface"))
    {
        const int deletedSurface = app.selectedSurface;
        device.delete_surface(deletedSurface);
        app.selectedSurface = device.nb_surface() == 0
            ? -1 : std::min(deletedSurface, device.nb_surface() - 1);
        markChanged(app);
    }
    ImGui::EndDisabled();

    std::vector<std::string> materials;
    GlassManager::singleton().list_available(materials);
    const char* types[] = {
        "stop", "reflect", "void", "image", "image_infinite", "perfect_lens", "perfect_mirror"
    };
    const int columnCount = 6 + (app.displayTilt ? 2 : 0) +
        (app.displayInnerDiameter ? 1 : 0) +
        (app.displayAspheric ? 4 : 0) + (app.displayComments ? 1 : 0);
    const ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("surface-table", columnCount, flags, ImVec2(0.0f, 0.0f)))
    {
        ImGui::TableSetupColumn("Surface");
        ImGui::TableSetupColumn("Medium", ImGuiTableColumnFlags_WidthFixed, 145.0f);
        ImGui::TableSetupColumn("Radius");
        ImGui::TableSetupColumn("Conic");
        ImGui::TableSetupColumn(app.relativeConvention ? "Thickness" : "Z");
        ImGui::TableSetupColumn("Diameter");
        if (app.displayTilt)
        {
            ImGui::TableSetupColumn("Tilt X");
            ImGui::TableSetupColumn("Tilt Y");
        }
        if (app.displayInnerDiameter) ImGui::TableSetupColumn("InnerDiameter");
        if (app.displayAspheric)
        {
            ImGui::TableSetupColumn("R4"); ImGui::TableSetupColumn("R6");
            ImGui::TableSetupColumn("R8"); ImGui::TableSetupColumn("R10");
        }
        if (app.displayComments) ImGui::TableSetupColumn("Comment");
        ImGui::TableHeadersRow();

        for (int i = 0; i < device.nb_surface(); ++i)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            int column = 0;
            ImGui::TableSetColumnIndex(column++);
            if (ImGui::Selectable("##surface", app.selectedSurface == i,
                ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap))
                app.selectedSurface = i;
            ImGui::SameLine();
            ImGui::Text("%d", i + 1);
            ImGui::TableSetColumnIndex(column++);
            const std::string currentType = device.type(i);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##medium", currentType.c_str()))
            {
                for (const char* type : types)
                    if (ImGui::Selectable(type, currentType == type))
                    {
                        device.set_type(i, type);
                        markChanged(app);
                    }
                for (const std::string& material : materials)
                    if (ImGui::Selectable(material.c_str(), currentType == material))
                    {
                        device.set_type(i, material);
                        markChanged(app);
                    }
                ImGui::EndCombo();
            }

            ImGui::TableSetColumnIndex(column++);
            double radius = device.get(i, RADIUS_CURVATURE);
            const bool infinite = std::abs(radius) >= RADIUS_CURVATURE_INFINITY / 2.0;
            int cloneReference = -1;
            double cloneGain = 1.0;
            const bool radiusCloned = device.get_clone(
                i, RADIUS_CURVATURE, cloneReference, cloneGain);
            if (radiusCloned)
                ImGui::TextDisabled("#%d%s", cloneReference + 1, cloneGain < 0.0 ? " (-)" : "");
            else if (infinite)
                ImGui::TextUnformatted("inf");
            else if (ImGui::InputDouble("##radius", &radius, 0.0, 0.0, "%.8g"))
            {
                device.set(i, RADIUS_CURVATURE, radius);
                markChanged(app);
            }
            if (!radiusCloned)
            {
                ImGui::SameLine();
                if (ImGui::SmallButton(infinite ? "Set" : "Inf"))
                {
                    device.set(i, RADIUS_CURVATURE, infinite ? 0.0 : RADIUS_CURVATURE_INFINITY);
                    markChanged(app);
                }
            }

            auto inputParameter = [&](eSurfaceParameter parameter, const char* label) {
                ImGui::TableSetColumnIndex(column++);
                double value = device.get(i, parameter);
                int reference = -1;
                double gain = 1.0;
                if (device.get_clone(i, parameter, reference, gain))
                {
                    ImGui::BeginDisabled();
                    ImGui::SetNextItemWidth(105.0f);
                    ImGui::InputDouble(label, &value, 0.0, 0.0, "%.8g");
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextDisabled("#%d%s", reference + 1, gain < 0.0 ? " (-)" : "");
                    return;
                }
                ImGui::SetNextItemWidth(105.0f);
                if (ImGui::InputDouble(label, &value, 0.0, 0.0, "%.8g"))
                {
                    device.set(i, parameter, value);
                    markChanged(app);
                }
            };
            inputParameter(CONIC, "##conic");
            inputParameter(app.relativeConvention ? THICK : Z, "##position");
            auto inputDiameter = [&](eSurfaceParameter valueParameter,
                eSurfaceParameter autoParameter, const char* label) {
                ImGui::TableSetColumnIndex(column++);
                bool automatic = i > 0 && device.get(i, autoParameter) != 0.0;
                if (i > 0)
                {
                    ImGui::TextUnformatted("Auto");
                    ImGui::SameLine();
                    if (ImGui::Checkbox(("##auto" + std::string(label)).c_str(), &automatic))
                    {
                        device.set(i, autoParameter, automatic ? 1.0 : 0.0);
                        markChanged(app);
                    }
                    ImGui::SameLine();
                }
                double value = device.get(i, valueParameter);
                int reference = -1;
                double gain = 1.0;
                const bool cloned = device.get_clone(i, valueParameter, reference, gain);
                ImGui::BeginDisabled(automatic || cloned);
                ImGui::SetNextItemWidth(105.0f);
                if (ImGui::InputDouble(label, &value, 0.0, 0.0, "%.8g") && !automatic && !cloned)
                {
                    device.set(i, valueParameter, value);
                    markChanged(app);
                }
                ImGui::EndDisabled();
                if (cloned)
                {
                    ImGui::SameLine();
                    ImGui::TextDisabled("#%d%s", reference + 1, gain < 0.0 ? " (-)" : "");
                }
            };
            inputDiameter(DIAMETER, AUTO_DIAMETER, "##diameter");
            if (app.displayTilt)
            {
                inputParameter(TILT_X, "##tilt-x");
                inputParameter(TILT_Y, "##tilt-y");
            }
            if (app.displayInnerDiameter)
                inputDiameter(INNER_DIAMETER, AUTO_INNER_DIAMETER, "##inner-diameter");
            if (app.displayAspheric)
            {
                inputParameter(R4, "##r4");
                inputParameter(R6, "##r6");
                inputParameter(R8, "##r8");
                inputParameter(R10, "##r10");
            }
            if (app.displayComments)
            {
                ImGui::TableSetColumnIndex(column++);
                char comment[256] = {};
                const std::string text = device.comment(i);
                std::snprintf(comment, sizeof(comment), "%s", text.c_str());
                ImGui::SetNextItemWidth(160.0f);
                if (ImGui::InputText("##comment", comment, sizeof(comment)))
                {
                    device.set_comment(i, comment);
                    markChanged(app);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

struct PlotTransform
{
    ImVec2 min;
    ImVec2 max;
    double left;
    double right;
    double bottom;
    double top;

    ImVec2 map(double x, double y) const
    {
        return ImVec2(
            min.x + static_cast<float>((x - left) / (right - left)) * (max.x - min.x),
            max.y - static_cast<float>((y - bottom) / (top - bottom)) * (max.y - min.y));
    }
};

PlotTransform makeTransform(const ImVec2& min, const ImVec2& max,
    double left, double right, double bottom, double top)
{
    if (right - left < 1e-9) { left -= 1.0; right += 1.0; }
    if (top - bottom < 1e-9) { bottom -= 1.0; top += 1.0; }
    const double centerX = (left + right) / 2.0;
    const double centerY = (bottom + top) / 2.0;
    const double scale = std::max((right - left) / std::max(1.0f, max.x - min.x),
        (top - bottom) / std::max(1.0f, max.y - min.y));
    const double halfWidth = scale * (max.x - min.x) / 2.0;
    const double halfHeight = scale * (max.y - min.y) / 2.0;
    return { min, max, centerX - halfWidth, centerX + halfWidth,
        centerY - halfHeight, centerY + halfHeight };
}

void drawOpticalLayout(App& app)
{
    if (!ImGui::Begin("Optical layout"))
    {
        ImGui::End();
        return;
    }
    const ImVec2 canvasMin = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.x = std::max(canvasSize.x, 100.0f);
    canvasSize.y = std::max(canvasSize.y, 100.0f);
    const ImVec2 canvasMax(canvasMin.x + canvasSize.x, canvasMin.y + canvasSize.y);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const ImU32 canvasColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
    const ImU32 borderColor = ImGui::GetColorU32(ImGuiCol_Border);
    const ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
    draw->AddRectFilled(canvasMin, canvasMax, canvasColor);
    draw->AddRect(canvasMin, canvasMax, borderColor);

    OpticalDevice& device = app.device;
    if (device.nb_surface() == 0)
    {
        draw->AddText(ImVec2(canvasMin.x + 14.0f, canvasMin.y + 14.0f),
            textColor, "Add surfaces to display the optical system");
        ImGui::Dummy(canvasSize);
        ImGui::End();
        return;
    }

    struct Point { double z; double x; };
    double minZ = device.get(0, Z);
    double maxZ = minZ;
    double maxRadius = 0.0;
    std::vector<std::vector<SurfaceProfilePoint>> profiles;
    profiles.reserve(static_cast<std::size_t>(device.nb_surface()));
    for (int i = 0; i < device.nb_surface(); ++i)
    {
        const double z = device.get(i, Z);
        const double radius = std::max(0.0, device.get(i, DIAMETER) / 2.0);
        minZ = std::min(minZ, z);
        maxZ = std::max(maxZ, z);
        maxRadius = std::max(maxRadius, radius);
        std::vector<SurfaceProfilePoint> profile = device.get_surface_profile(i);
        for (const SurfaceProfilePoint& point : profile)
        {
            minZ = std::min(minZ, point.z);
            maxZ = std::max(maxZ, point.z);
            maxRadius = std::max(maxRadius, std::abs(point.x));
            maxRadius = std::max(maxRadius, std::abs(point.y));
        }
        profiles.push_back(std::move(profile));
    }

    struct LightSegment { Point from; Point to; ImU32 color; };
    std::vector<LightSegment> lightSegments;
    const double selectedAngle =
        app.offAxisPercent * device.half_field_of_view() / 100.0;

    auto includePhotons = [&](const std::vector<Photon>& photons) {
        for (const Photon& photon : photons)
        {
            if (!photon.is_valid())
                continue;
            minZ = std::min(minZ, photon.z);
            maxZ = std::max(maxZ, photon.z);
            maxRadius = std::max(maxRadius, std::abs(photon.x));
        }
    };

    Light light;
    device.compute_light(&light, -1, selectedAngle, 17, 1);
    std::vector<Photon> previousPhotons = light.get_all_photons();
    includePhotons(previousPhotons);

    for (int surface = 0; surface < device.nb_surface(); ++surface)
    {
        device.compute_light(&light, surface, selectedAngle, 17, 1);
        const std::vector<Photon> current = light.get_all_photons();
        includePhotons(current);
        const std::size_t count = std::min(previousPhotons.size(), current.size());
        for (std::size_t ray = 0; ray < count; ++ray)
        {
            if (!previousPhotons[ray].is_valid() || !current[ray].is_valid())
                continue;
            const ImU32 color = toImColor(static_cast<unsigned int>(
                light.get_visual_color(previousPhotons[ray].lambda(),
                    app.theme != UiTheme::Light)));
            lightSegments.push_back({
                { previousPhotons[ray].z, previousPhotons[ray].x },
                { current[ray].z, current[ray].x },
                color
            });
        }
        previousPhotons = current;
    }

    const ImVec2 plotMin(canvasMin.x + 10.0f, canvasMin.y + 10.0f);
    const ImVec2 plotMax(canvasMax.x - 10.0f, canvasMax.y - 10.0f);
    constexpr float fitMarginRatio = 0.05f;
    const ImVec2 fitMargin(
        (plotMax.x - plotMin.x) * fitMarginRatio,
        (plotMax.y - plotMin.y) * fitMarginRatio);
    const ImVec2 fitMin(plotMin.x + fitMargin.x, plotMin.y + fitMargin.y);
    const ImVec2 fitMax(plotMax.x - fitMargin.x, plotMax.y - fitMargin.y);
    PlotTransform transform = makeTransform(fitMin, fitMax, minZ, maxZ, -maxRadius, maxRadius);
    const double centerX = (transform.left + transform.right) / 2.0;
    const double centerY = (transform.bottom + transform.top) / 2.0;
    const double halfX = (transform.right - transform.left) / (2.0 * app.layoutZoom);
    const double halfY = (transform.top - transform.bottom) / (2.0 * app.layoutZoom);
    transform.left = centerX - halfX + app.layoutPan.x;
    transform.right = centerX + halfX + app.layoutPan.x;
    transform.bottom = centerY - halfY + app.layoutPan.y;
    transform.top = centerY + halfY + app.layoutPan.y;

    if (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(plotMin, plotMax) &&
        ImGui::GetIO().MouseWheel != 0.0f)
    {
        const double previousZoom = app.layoutZoom;
        const double newZoom = std::clamp(previousZoom *
            (ImGui::GetIO().MouseWheel > 0.0f ? 1.15 : 0.87),
            0.1, kMaxOpticalLayoutZoom);
        const double zoomDelta = 1.0 - previousZoom / newZoom;
        const ImVec2 mousePos = ImGui::GetIO().MousePos;
        const double mouseX = (mousePos.x - transform.min.x) /
            std::max(1.0f, transform.max.x - transform.min.x);
        const double mouseY = (mousePos.y - transform.min.y) /
            std::max(1.0f, transform.max.y - transform.min.y);
        app.layoutPan.x += static_cast<float>(halfX * (2.0 * mouseX - 1.0) * zoomDelta);
        app.layoutPan.y += static_cast<float>(halfY * (1.0 - 2.0 * mouseY) * zoomDelta);
        app.layoutZoom = newZoom;
    }
    if (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(plotMin, plotMax) &&
        !ImGui::IsAnyItemHovered() &&
        (ImGui::IsMouseDragging(ImGuiMouseButton_Left) ||
            ImGui::IsMouseDragging(ImGuiMouseButton_Middle)))
    {
        const ImVec2 delta = ImGui::GetIO().MouseDelta;
        app.layoutPan.x -= delta.x * static_cast<float>(
            (transform.right - transform.left) / std::max(1.0f, plotMax.x - plotMin.x));
        app.layoutPan.y += delta.y * static_cast<float>(
            (transform.top - transform.bottom) / std::max(1.0f, plotMax.y - plotMin.y));
    }
    draw->PushClipRect(plotMin, plotMax, true);
    for (std::size_t i = 1; i < profiles.size(); ++i)
    {
        const unsigned int color = GlassManager::singleton().solid_color(
            device.type(static_cast<int>(i - 1)));
        if (color == 0xffffff)
            continue;

        const auto& previous = profiles[i - 1];
        const auto& current = profiles[i];
        if (previous.size() < 2 || previous.size() != current.size())
            continue;

        const ImU32 fill = toImColor(color);
        for (std::size_t sample = 1; sample < current.size(); ++sample)
            drawGlassSection(draw,
                transform.map(current[sample - 1].z, current[sample - 1].y),
                transform.map(current[sample].z, current[sample].y),
                transform.map(previous[sample].z, previous[sample].y),
                transform.map(previous[sample - 1].z, previous[sample - 1].y),
                fill);
    }

    for (int i = 0; i < device.nb_surface(); ++i)
    {
        const auto& profile = profiles[static_cast<std::size_t>(i)];
        if (profile.size() < 2)
            continue;
        std::vector<ImVec2> points;
        points.reserve(profile.size());
        for (const SurfaceProfilePoint& point : profile)
            points.push_back(transform.map(point.z, point.y));
        draw->AddPolyline(points.data(), static_cast<int>(points.size()),
            textColor, ImDrawFlags_None, 1.5f);
    }

    for (std::size_t i = 1; i < profiles.size(); ++i)
    {
        const unsigned int color = GlassManager::singleton().solid_color(
            device.type(static_cast<int>(i - 1)));
        const auto& previous = profiles[i - 1];
        const auto& current = profiles[i];
        if (color == 0xffffff || previous.empty() || current.empty())
            continue;

        draw->AddLine(transform.map(previous.front().z, previous.front().y),
            transform.map(current.front().z, current.front().y), textColor, 1.5f);
        draw->AddLine(transform.map(previous.back().z, previous.back().y),
            transform.map(current.back().z, current.back().y), textColor, 1.5f);
    }

    for (const LightSegment& segment : lightSegments)
        draw->AddLine(transform.map(segment.from.z, segment.from.x),
            transform.map(segment.to.z, segment.to.x), segment.color, 1.0f);
    draw->PopClipRect();

    ImGui::SetCursorScreenPos(ImVec2(canvasMin.x + 12.0f, canvasMax.y - 27.0f));
    if (ImGui::SmallButton("Fit view"))
        fitOpticalLayout(app);
    ImGui::SameLine();
    ImGui::TextDisabled("Wheel: zoom at cursor | Left/middle drag: pan");
    ImGui::SetCursorScreenPos(canvasMin);
    ImGui::Dummy(canvasSize);
    ImGui::End();
}

void drawSpotDiagram(App& app)
{
    if (!ImGui::Begin("Spot diagram"))
    {
        ImGui::End();
        return;
    }
    const ImVec2 canvasMin = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();
    size.x = std::max(size.x, 100.0f);
    size.y = std::max(size.y, 100.0f);
    const ImVec2 canvasMax(canvasMin.x + size.x, canvasMin.y + size.y);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(canvasMin, canvasMax, ImGui::GetColorU32(ImGuiCol_FrameBg));
    draw->AddRect(canvasMin, canvasMax, ImGui::GetColorU32(ImGuiCol_Border));

    if (app.device.nb_surface() > 0)
    {
        Light light;
        const double tilt = app.offAxisPercent * app.device.half_field_of_view() / 100.0;
        app.device.compute_light(&light, app.device.nb_surface() - 1, tilt, 35, 35);
        if (light.is_valid())
        {
            struct SpotPoint { double x; double y; double lambda; };
            const bool imageInfinite = light.is_image_infinite();
            const std::vector<Photon> photons = light.get_all_photons();
            std::vector<SpotPoint> points;
            points.reserve(photons.size());

            double minX = std::numeric_limits<double>::max();
            double maxX = std::numeric_limits<double>::lowest();
            double minY = std::numeric_limits<double>::max();
            double maxY = std::numeric_limits<double>::lowest();
            auto include = [&](double x, double y) {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            };
            for (const Photon& photon : photons)
            {
                if (!photon.is_valid())
                    continue;
                const double x = imageInfinite ? -photon.angley : -photon.y;
                const double y = imageInfinite ? photon.anglex : photon.x;
                points.push_back({ x, y, photon.lambda() });
                include(x, y);
            }

            double spotCenterX = 0.0;
            double spotCenterY = 0.0;
            light.get_spot_center(spotCenterX, spotCenterY);
            const double circleCenterX = -spotCenterY;
            const double circleCenterY = spotCenterX;
            const double airyRadius = light.airy_radius();
            include(circleCenterX - airyRadius, circleCenterY - airyRadius);
            include(circleCenterX + airyRadius, circleCenterY + airyRadius);

            double centerX = (minX + maxX) * 0.5;
            double centerY = (minY + maxY) * 0.5;
            double spanX = std::max(maxX - minX, 1e-12);
            double spanY = std::max(maxY - minY, 1e-12);
            spanX *= 1.1;
            spanY *= 1.1;

            const ImVec2 plotMin(canvasMin.x + 10.0f, canvasMin.y + 32.0f);
            const ImVec2 plotMax(canvasMax.x - 10.0f, canvasMax.y - 34.0f);
            const float plotWidth = std::max(1.0f, plotMax.x - plotMin.x);
            const float plotHeight = std::max(1.0f, plotMax.y - plotMin.y);
            const double pixelsPerUnit = std::min(plotWidth / spanX, plotHeight / spanY) *
                app.spotZoom;
            const ImVec2 plotCenter((plotMin.x + plotMax.x) * 0.5f,
                (plotMin.y + plotMax.y) * 0.5f);
            auto mapPoint = [&](double x, double y) {
                return ImVec2(
                    plotCenter.x + static_cast<float>((x - centerX) * pixelsPerUnit),
                    plotCenter.y - static_cast<float>((y - centerY) * pixelsPerUnit));
            };

            if (ImGui::IsMouseHoveringRect(plotMin, plotMax) &&
                ImGui::GetIO().MouseWheel != 0.0f)
            {
                app.spotZoom = std::clamp(app.spotZoom *
                    (ImGui::GetIO().MouseWheel > 0.0f ? 1.15 : 0.87), 0.1, 50.0);
            }

            draw->PushClipRect(plotMin, plotMax, true);
            const ImU32 diffractionCircleColor = app.theme == UiTheme::Light
                ? IM_COL32(0, 0, 0, 255) : IM_COL32(255, 255, 255, 255);
            for (const SpotPoint& point : points)
                draw->AddCircleFilled(mapPoint(point.x, point.y), 2.0f,
                    toImColor(static_cast<unsigned int>(light.get_visual_color(point.lambda,
                        app.theme != UiTheme::Light))));
            draw->AddCircle(mapPoint(circleCenterX, circleCenterY),
                static_cast<float>(airyRadius * pixelsPerUnit), diffractionCircleColor, 64, 2.0f);
            draw->PopClipRect();

            ImGui::SetCursorScreenPos(ImVec2(canvasMin.x + 10.0f, canvasMin.y + 6.0f));
            const double unitScale = imageInfinite ? 1.0 : 1000.0;
            const char* unit = imageInfinite ? "deg" : "um";
            if (imageInfinite)
                ImGui::Text("Spot: %.5g %s | Airy diameter: %.5g %s | Spot/Airy: %.5g",
                    light.spot_size() * unitScale, unit, 2.0 * airyRadius * unitScale,
                    unit, light.spot_vs_airy());
            else
                ImGui::Text("Spot: %.5g %s | Airy diameter: %.5g %s | F/D: %.5g",
                    light.spot_size() * unitScale, unit, 2.0 * airyRadius * unitScale,
                    unit, light.get_FD());
        }
    }
    ImGui::SetCursorScreenPos(ImVec2(canvasMin.x + 10.0f, canvasMax.y - 28.0f));
    if (ImGui::SmallButton("Fit view"))
        app.spotZoom = 1.0;
    ImGui::SameLine();
    ImGui::TextDisabled("Wheel: zoom");
    ImGui::SetCursorScreenPos(canvasMin);
    ImGui::Dummy(size);
    ImGui::End();
}

void drawImageQuality(App& app)
{
    if (!ImGui::Begin("Image quality"))
    {
        ImGui::End();
        return;
    }
    const ImageQuality quality = app.device.get_image_quality();
    const int angleCount = quality.nb_angles();
    auto selectRow = [&](int row) {
        app.selectedImageQualityRow = row;
        const double angle = quality.vdAngles[static_cast<std::size_t>(row)];
        app.offAxisPercent = std::abs(app.device.half_field_of_view()) > 1e-12
            ? 100.0 * angle / app.device.half_field_of_view() : 0.0;
        app.device.set_parameter("showLightOffAxis", app.offAxisPercent);
    };
    ImGui::Text("Airy size: %.6g %s    F-number: %.6g",
        quality.dAirySize * (quality.isImageInfinite ? 1.0 : 1000.0),
        quality.isImageInfinite ? "deg" : "um", quality.dFNumber);
    if (ImGui::BeginTable("quality-table", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_Resizable, ImVec2(0.0f, 0.0f)))
    {
        ImGui::TableSetupColumn("Angle (deg)");
        ImGui::TableSetupColumn(quality.isImageInfinite ? "Out angle" : "Dist (mm)");
        ImGui::TableSetupColumn(quality.isImageInfinite ? "Spot (deg)" : "Spot (um)");
        ImGui::TableSetupColumn(quality.isImageInfinite ? "Spot/eye limit" : "Spot/Airy");
        ImGui::TableSetupColumn("Vignetting");
        ImGui::TableHeadersRow();
        for (int i = 0; i < angleCount; ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            const bool selected = app.selectedImageQualityRow == i;
            if (ImGui::Selectable(("##angle" + std::to_string(i)).c_str(), selected,
                ImGuiSelectableFlags_SpanAllColumns))
                selectRow(i);
            if (ImGui::IsItemFocused())
            {
                int row = std::clamp(app.selectedImageQualityRow, 0, angleCount - 1);
                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow, true))
                    row = std::max(0, row - 1);
                else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow, true))
                    row = std::min(angleCount - 1, row + 1);
                if (row != app.selectedImageQualityRow)
                    selectRow(row);
            }
            ImGui::SameLine();
            ImGui::Text("%.5g", quality.vdAngles[static_cast<std::size_t>(i)]);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.5g", quality.vdDist[static_cast<std::size_t>(i)]);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.5g", quality.vdSpotSize[static_cast<std::size_t>(i)] *
                (quality.isImageInfinite ? 1.0 : 1000.0));
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.5g", quality.vdSpotvsAiry[static_cast<std::size_t>(i)]);
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%.5g %%", quality.vdVignetting[static_cast<std::size_t>(i)]);
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
void drawOpticalPanels(App& app)
{
    drawSurfaces(app);
    drawOpticalLayout(app);
    drawSpotDiagram(app);
    drawImageQuality(app);
}

}
