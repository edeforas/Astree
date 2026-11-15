// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include <cfloat>

namespace astree_imgui_app
{
void drawNotes(App& app)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.96f, 0.70f, 1.0f));
    if (!ImGui::Begin("Notes"))
    {
        ImGui::End();
        ImGui::PopStyleColor();
        return;
    }
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.93f, 0.58f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.0f, 0.95f, 0.66f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 0.94f, 0.62f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.22f, 0.19f, 0.08f, 1.0f));
    const bool notesChanged = ImGui::InputTextMultiline("##note", app.noteBuffer,
        sizeof(app.noteBuffer), ImVec2(-FLT_MIN, -FLT_MIN));
    ImGui::PopStyleColor(4);
    if (notesChanged)
    {
        app.device.set_note(app.noteBuffer);
        markChanged(app);
    }
    ImGui::End();
    ImGui::PopStyleColor();
}
}
