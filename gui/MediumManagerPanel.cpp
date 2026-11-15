// this file is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras (the author) mailto: etienne.deforas@gmail.com


#include "App.h"
#include "Glass.h"
#include "GlassManager.h"
#include <memory>
#include <string>
#include <vector>

namespace astree_imgui_app
{
void drawMediumManager(App& app)
{
    if (!app.showMediumManager)
        return;
    ImGui::SetNextWindowSize(ImVec2(420.0f, 500.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Medium manager", &app.showMediumManager))
    {
        std::vector<std::string> materials;
        std::vector<std::string> catalogs;
        GlassManager::singleton().list_available(materials);
        GlassManager::singleton().list_catalogs(catalogs);
        ImGui::Text("Catalogs: %zu", catalogs.size());
        for (const std::string& catalog : catalogs)
            ImGui::BulletText("%s", catalog.c_str());
        ImGui::Separator();
        ImGui::Text("Available media: %zu", materials.size());
        if (ImGui::BeginTable("media-table", 7,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Resizable, ImVec2(0.0f, 0.0f)))
        {
            const char* headers[] = { "Maker", "Name", "Formula", "Nd", "Vd", "Ne", "Ve" };
            for (const char* header : headers)
                ImGui::TableSetupColumn(header);
            ImGui::TableHeadersRow();
            for (const std::string& name : materials)
            {
                std::unique_ptr<Glass> medium(GlassManager::singleton().create(name));
                if (!medium)
                    continue;
                double nd = 0.0, vd = 0.0, ne = 0.0, ve = 0.0;
                medium->compute_NdVd(nd, vd);
                medium->compute_NeVe(ne, ve);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(medium->maker().c_str());
                ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(medium->name().c_str());
                ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted(medium->formula().c_str());
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.5f", nd);
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.3f", vd);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.5f", ne);
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.3f", ve);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}
}
