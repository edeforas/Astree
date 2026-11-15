// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"

namespace astree_imgui_app
{
void drawResetSettingsPrompt(App& app)
{
    if (app.confirmResetSettings)
    {
        ImGui::OpenPopup("Reset Settings");
        app.confirmResetSettings = false;
    }
    if (!ImGui::BeginPopupModal("Reset Settings", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::TextUnformatted("Reset all UI settings to their defaults?");
    ImGui::TextUnformatted("The optical design will not be changed.");
    if (ImGui::Button("Reset"))
    {
        resetUiSettings(app);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
}
}
