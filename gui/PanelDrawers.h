#pragma once

namespace astree_imgui_app
{
struct App;

void drawLightProperties(App& app);
void drawNotes(App& app);
void drawOptimizer(App& app);
void drawMediumManager(App& app);
void drawCloneEditor(App& app);
void drawScaleDialog(App& app);
void drawRevertDialog(App& app);
void drawAbout(App& app);
void drawPendingActionPrompt(App& app);
void drawResetSettingsPrompt(App& app);
void drawStatusBar(const App& app);
}
