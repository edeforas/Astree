// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include <algorithm>
#include <iterator>
#include <string>

namespace astree_imgui_app
{
void drawLightProperties(App& app)
{
    if (!ImGui::Begin("Light"))
    {
        ImGui::End();
        return;
    }
    bool changed = false;
    ImGui::TextUnformatted("Half field of view (deg)");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::CalcItemWidth() * 0.5f);
    changed |= ImGui::InputDouble("##half-field-of-view", &app.fov, 0.0, 0.0, "%.6g");
    ImGui::TextUnformatted("Intermediate angles");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::CalcItemWidth() * 0.5f);
    changed |= ImGui::InputInt("##intermediate-angles", &app.intermediateAngles);
    app.intermediateAngles = std::max(1, app.intermediateAngles);
    ImGui::Text("Wavelengths");
    const std::size_t colorOrder[] = { 1, 3, 4, 5, 0, 2, 6 };
    for (std::size_t i = 0; i < std::size(colorOrder); ++i)
    {
        if (i != 0 && i != 4) ImGui::SameLine();
        const std::size_t color = colorOrder[i];
        ImGui::TextUnformatted(kColorNames[color]);
        ImGui::SameLine();
        ImGui::PushID(static_cast<int>(color));
        if (ImGui::Checkbox("##wavelength", &app.colors[color]))
            changed = true;
        ImGui::PopID();
    }
    std::string colorList;
    for (std::size_t i = 0; i < app.colors.size(); ++i)
        if (app.colors[i])
            colorList += std::string(kColorNames[i]) + ".";
    if (changed)
    {
        app.device.set_half_field_of_view(app.fov);
        app.device.set_nb_intermediate_angles(app.intermediateAngles);
        app.device.set_light_colors(colorList);
        app.device.get_image_quality();
        markChanged(app);
    }
    ImGui::End();
}
}
