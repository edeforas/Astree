// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include <string>

namespace astree_imgui_app
{
void drawStatusBar(const App& app)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float height = ImGui::GetFrameHeight();
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x,
        viewport->WorkPos.y + viewport->WorkSize.y - height));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, height));
    ImGui::SetNextWindowViewport(viewport->ID);
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    if (ImGui::Begin("Status bar", nullptr, flags))
    {
        const std::string name = app.fileName.empty()
            ? "Untitled" : fs::path(app.fileName).filename().string();
        ImGui::Text("%s%s  |  %s", app.dirty ? "* " : "", name.c_str(), app.status.c_str());
    }
    ImGui::End();
}
}
