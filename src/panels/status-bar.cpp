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

    // Dynamic operation badges (ROI, Crop, Rotation, Skew)
    // Separator before badges if any are active
    auto &controls = program::WindowState::controlsState;
    bool show_roi   = controls.selectionFlags.roi_enabled || controls.selection.has_roi;
    bool show_crop  = controls.selectionFlags.crop_enabled;
    // Compute current rotation angle from transformation matrix if available
    float angle_deg = 0.f;
    bool has_rotation = false;
    if (asset && !asset->transformation.empty() && asset->transformation.rows == 3 && asset->transformation.cols == 3) {
        float a = asset->transformation.at<float>(0,0);
        float b = asset->transformation.at<float>(0,1);
        float c = asset->transformation.at<float>(1,0);
        float d = asset->transformation.at<float>(1,1);
        angle_deg = std::atan2(b, a) * 180.0f / 3.14159265f; // approximate ignoring shear
        has_rotation = std::fabs(angle_deg) > 0.01f || controls.geoTransformFlags.rotation_center_enabled;
    }
    // Only show rotation/skew badges while geo transform panel active
    bool show_rotation = controls.geoTransformEnabled && has_rotation;
    bool show_skew = controls.geoTransformEnabled && controls.geoTransformFlags.skew_enabled;

    if (show_roi || show_crop || show_rotation || show_skew) {
        ImGui::SameLine();
        ImGui::TextUnformatted("|");
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec4 accent = st.Colors[ImGuiCol_CheckMark];
    // Slightly translucent background derived from accent
    auto mkBg = [&](float alpha){ return ImVec4(accent.x, accent.y, accent.z, alpha); };
    ImVec4 badgeBg = mkBg(0.25f);
    ImVec4 badgeBgStrong = mkBg(0.35f);
    ImVec4 badgeText = st.Colors[ImGuiCol_Text];
    float padX = 6.f, padY = 3.f, rounding = 6.f;

    auto Badge = [&](const char* icon, const std::string &text, bool strong){
        ImGui::SameLine();
        ImVec2 start = ImGui::GetCursorScreenPos();
        std::string content = std::string(icon) + " " + text;
        ImVec2 textSize = ImGui::CalcTextSize(content.c_str());
        ImVec2 rectMax(start.x + textSize.x + padX*2, start.y + textSize.y + padY*2);
        dl->AddRectFilled(start, rectMax, ImGui::ColorConvertFloat4ToU32(strong? badgeBgStrong : badgeBg), rounding);
        // Optional subtle border
        dl->AddRect(start, rectMax, ImGui::ColorConvertFloat4ToU32(accent), rounding);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padX);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + padY);
        ImGui::PushStyleColor(ImGuiCol_Text, badgeText);
        ImGui::TextUnformatted(content.c_str());
        ImGui::PopStyleColor();
    };

    if (show_roi && asset) {
        int sx = (int)std::round(controls.selection.start_img.x);
        int sy = (int)std::round(controls.selection.start_img.y);
        int ex = (int)std::round(controls.selection.end_img.x);
        int ey = (int)std::round(controls.selection.end_img.y);
        int rw = std::abs(ex - sx);
        int rh = std::abs(ey - sy);
        std::string roiText = rw > 0 && rh > 0 ? ("(" + std::to_string(sx) + "," + std::to_string(sy) + ") " + std::to_string(rw) + "x" + std::to_string(rh)) : "Active";
        Badge(ICON_FA_VECTOR_SQUARE, "ROI " + roiText, false);
    }
    if (show_crop) {
        Badge(ICON_FA_CROP, "Crop Mode", true);
    }
    if (show_rotation && asset) {
        int cx = (int)std::round(asset->rotation_center.x);
        int cy = (int)std::round(asset->rotation_center.y);
        std::string rotText = "(" + std::to_string(cx) + "," + std::to_string(cy) + ") " + std::to_string((int)std::round(angle_deg)) + "\xC2\xB0"; // degree symbol
        Badge(ICON_FA_ARROW_ROTATE_RIGHT, rotText, false);
    }
    if (show_skew) {
        Badge(ICON_FA_ARROWS_LEFT_RIGHT, "Skew", false);
    }

    ImGui::PopStyleVar(2);
}
