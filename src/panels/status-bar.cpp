#include "panels/status-bar.hpp"
#include "shared/state.hpp"
#include "acquisitor/acquisitor.hpp"
#include <cmath>

void IStatusBarPanel::draw() {
    std::shared_ptr<toolbox::Asset> asset = program::getChosenAsset();

    ImGuiStyle &st = ImGui::GetStyle();
    ImVec4 textCol = st.Colors[ImGuiCol_Text];
    ImVec4 muted   = st.Colors[ImGuiCol_TextDisabled];

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 0));

    // Left side: mouse coords
    ImGui::PushStyleColor(ImGuiCol_Text, textCol);
    ImGui::TextUnformatted(ICON_FA_ARROW_POINTER);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    if (asset && asset->SDL_texture) {
        ImVec2 mouse = ImGui::GetIO().MousePos;
        ImVec2 topLeft = asset->position;
        ImVec2 dispSize = asset->size;
        bool inImage = mouse.x >= topLeft.x && mouse.x <= topLeft.x + dispSize.x &&
                       mouse.y >= topLeft.y && mouse.y <= topLeft.y + dispSize.y;
        if (inImage) {
            float sx = (float)asset->displayed_image.cols / dispSize.x;
            float sy = (float)asset->displayed_image.rows / dispSize.y;
            int ix = (int)std::floor((mouse.x - topLeft.x) * sx);
            int iy = (int)std::floor((mouse.y - topLeft.y) * sy);
            ix = std::max(0, std::min(ix, asset->displayed_image.cols - 1));
            iy = std::max(0, std::min(iy, asset->displayed_image.rows - 1));
            ImGui::Text("%d, %d px", ix, iy);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, muted);
            ImGui::TextUnformatted("--, --");
            ImGui::PopStyleColor();
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, muted);
        ImGui::TextUnformatted("--, --");
        ImGui::PopStyleColor();
    }

    // Separator
    ImGui::SameLine();
    ImGui::TextUnformatted("|");
    ImGui::SameLine();

    // Zoom percent
    ImGui::PushStyleColor(ImGuiCol_Text, textCol);
    ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    if (asset && asset->SDL_texture) {
        float scaleX = asset->size.x > 0 ? (asset->size.x / (float)asset->displayed_image.cols) : 0.0f;
        float scaleY = asset->size.y > 0 ? (asset->size.y / (float)asset->displayed_image.rows) : 0.0f;
        float scale = (scaleX + scaleY) * 0.5f; // average scale
        int percent = (int)std::round(scale * 100.0f);
        ImGui::Text("%d%%", percent);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, muted);
        ImGui::TextUnformatted("--");
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar(2);
}
