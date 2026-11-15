// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include <algorithm>

namespace astree_imgui_app
{
void drawCloneEditor(App& app)
{
    if (!app.showCloneEditor)
        return;
    if (ImGui::Begin("Clone parameter", &app.showCloneEditor))
    {
        const int surfaceCount = app.device.nb_surface();
        if (surfaceCount < 2)
            ImGui::TextUnformatted("Add at least two surfaces to create a clone.");
        else
        {
            app.cloneSurface = std::clamp(app.cloneSurface, 2, surfaceCount);
            app.cloneReference = std::clamp(app.cloneReference, 1, app.cloneSurface - 1);
            ImGui::InputInt("Target surface", &app.cloneSurface);
            app.cloneSurface = std::clamp(app.cloneSurface, 2, surfaceCount);
            ImGui::InputInt("Reference surface", &app.cloneReference);
            app.cloneReference = std::clamp(app.cloneReference, 1, app.cloneSurface - 1);
            ImGui::Combo("Parameter", &app.cloneParameter, kCloneParameters, 9);
            ImGui::Checkbox("Negative gain", &app.cloneNegative);
            ImGui::TextDisabled("Clones must reference an earlier surface.");

            static const eSurfaceParameter cloneParameters[] = {
                RADIUS_CURVATURE, CONIC, THICK, DIAMETER, INNER_DIAMETER, R4, R6, R8, R10
            };
            const eSurfaceParameter parameter =
                app.cloneParameter == 2 && !app.relativeConvention
                    ? Z : cloneParameters[app.cloneParameter];
            if (ImGui::Button("Apply clone"))
            {
                app.device.set_clone(app.cloneSurface - 1, parameter,
                    app.cloneReference - 1, app.cloneNegative ? -1.0 : 1.0);
                markChanged(app, "Parameter clone updated");
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove clone"))
            {
                app.device.set_clone(app.cloneSurface - 1, parameter, -1, 0.0);
                markChanged(app, "Parameter clone removed");
            }
        }
    }
    ImGui::End();
}
}
