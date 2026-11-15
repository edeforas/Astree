// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"

namespace astree_imgui_app
{
void drawPendingActionPrompt(App& app)
{
    if (app.confirmPendingAction)
    {
        ImGui::OpenPopup("Unsaved changes");
        app.confirmPendingAction = false;
    }
    if (ImGui::BeginPopupModal("Unsaved changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Save changes before continuing?");
        if (ImGui::Button("Save"))
        {
            if (app.fileName.empty())
            {
                ImGui::CloseCurrentPopup();
                openFilePicker(app, true, false);
            }
            else if (saveDesign(app, app.fileName))
            {
                executePendingAction(app);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard"))
        {
            executePendingAction(app);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            app.pendingAction = PendingAction::None;
            app.pendingPath.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
}
