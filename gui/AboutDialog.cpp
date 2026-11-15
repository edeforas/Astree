// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"

namespace astree_imgui_app
{
void drawAbout(App& app)
{
    if (app.showAbout)
    {
        ImGui::OpenPopup("About Astree");
        app.showAbout = false;
    }
    if (ImGui::BeginPopupModal("About Astree", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Astree optical design");
        ImGui::Text("Dear ImGui interface");
        ImGui::Text("Copyright Etienne de Foras");
        ImGui::Text("GNU LGPL version 3 or later");
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
}
