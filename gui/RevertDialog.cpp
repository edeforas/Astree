// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include "DeviceRevert.h"
#include <algorithm>

namespace astree_imgui_app
{
void drawRevertDialog(App& app)
{
    if (app.showRevert)
    {
        ImGui::OpenPopup("Revert design");
        app.showRevert = false;
    }
    if (ImGui::BeginPopupModal("Revert design", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const int surfaceCount = app.device.nb_surface();
        const int maxSurface = std::max(1, surfaceCount);
        ImGui::InputInt("First surface", &app.revertFirst);
        ImGui::InputInt("Last surface", &app.revertLast);
        app.revertFirst = std::clamp(app.revertFirst, 1, maxSurface);
        app.revertLast = std::clamp(app.revertLast, 1, maxSurface);
        ImGui::BeginDisabled(surfaceCount == 0);
        if (ImGui::Button("Apply"))
        {
            DeviceRevert revert;
            if (revert.revert(&app.device, std::min(app.revertFirst, app.revertLast) - 1,
                std::max(app.revertFirst, app.revertLast) - 1))
                markChanged(app, "Design reverted");
            else
                app.status = "Revert failed";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
}
