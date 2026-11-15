// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include "DeviceScaling.h"
#include <cmath>

namespace astree_imgui_app
{
void drawScaleDialog(App& app)
{
    if (app.showScale)
    {
        ImGui::OpenPopup("Scale design");
        app.showScale = false;
    }
    if (ImGui::BeginPopupModal("Scale design", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::RadioButton("Factor", &app.scaleMode, 0);
        if (app.scaleMode == 0)
            ImGui::InputDouble("Scale factor", &app.scaleFactor);
        ImGui::RadioButton("Percentage", &app.scaleMode, 1);
        if (app.scaleMode == 1)
            ImGui::InputDouble("Percentage", &app.scalePercentage);
        ImGui::RadioButton("Set size", &app.scaleMode, 2);
        if (app.scaleMode == 2)
        {
            ImGui::InputDouble("Old size", &app.scaleOldSize);
            ImGui::InputDouble("New size", &app.scaleNewSize);
        }
        ImGui::Checkbox("Scale diameters", &app.scaleDiameter);
        ImGui::SameLine();
        ImGui::Checkbox("Scale focal lengths", &app.scaleFocal);
        if (ImGui::Button("Apply"))
        {
            double ratio = app.scaleFactor;
            if (app.scaleMode == 1)
                ratio = app.scalePercentage / 100.0;
            else if (app.scaleMode == 2 && app.scaleOldSize > 0.0)
                ratio = app.scaleNewSize / app.scaleOldSize;
            else if (app.scaleMode == 2)
                ratio = 0.0;
            if (std::isfinite(ratio) && ratio > 0.0)
            {
                DeviceScaling scaling;
                if (scaling.scale(&app.device, ratio, app.scaleDiameter, app.scaleFocal))
                    markChanged(app, "Design scaled");
                else
                    app.status = "Scaling failed";
                ImGui::CloseCurrentPopup();
            }
            else
                app.status = "Scale ratio must be positive and finite";
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
}
