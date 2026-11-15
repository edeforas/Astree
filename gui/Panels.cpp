// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include "PanelDrawers.h"

#include <GLFW/glfw3.h>
#include "imgui_internal.h"

namespace astree_imgui_app
{
void drawUi(App& app)
{
    if (ImGui::IsKeyPressed(ImGuiKey_F11))
        toggleMaximizedWindow(glfwGetCurrentContext(), app);
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        if (app.fileName.empty())
            openFilePicker(app, true, false);
        else
            saveDesign(app, app.fileName);
    }
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImGuiID dockspaceId = ImGui::DockSpaceOverViewport(
        0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
    if (app.initializeDockLayout)
    {
        app.initializeDockLayout = false;
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->WorkSize);
        ImGuiID upper = 0;
        ImGuiID bottom = 0;
        ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.30f, &bottom, &upper);

        ImGuiID imageQuality = 0;
        ImGuiID surfaces = 0;
        ImGui::DockBuilderSplitNode(bottom, ImGuiDir_Right, 1.0f / 3.0f,
            &imageQuality, &surfaces);

        ImGuiID left = 0;
        ImGuiID center = 0;
        ImGui::DockBuilderSplitNode(upper, ImGuiDir_Left, 0.25f, &left, &center);
        ImGuiID leftTop = 0;
        ImGuiID leftBottom = 0;
        ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.5f, &leftBottom, &leftTop);

        ImGui::DockBuilderDockWindow("Surfaces", surfaces);
        ImGui::DockBuilderDockWindow("Image quality", imageQuality);
        ImGui::DockBuilderDockWindow("Optical layout", center);
        ImGui::DockBuilderDockWindow("Spot diagram", leftTop);
        ImGui::DockBuilderDockWindow("Notes", leftBottom);
        ImGui::DockBuilderDockWindow("Optimizer", leftBottom);
        ImGui::DockBuilderDockWindow("Light", leftBottom);
        ImGui::DockBuilderFinish(dockspaceId);
    }
    drawFileMenu(app);
    drawOpticalPanels(app);
    drawLightProperties(app);
    drawOptimizer(app);
    drawNotes(app);
    drawCloneEditor(app);
    drawMediumManager(app);
    drawScaleDialog(app);
    drawRevertDialog(app);
    drawAbout(app);
    drawPendingActionPrompt(app);
    drawResetSettingsPrompt(app);
    drawFilePicker(app);
    drawStatusBar(app);
}
}
