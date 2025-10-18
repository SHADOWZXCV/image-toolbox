#include "panels/menubar.hpp"

void IMenuBarPanel::pre_draw() {}
void IMenuBarPanel::handle_events() {}

void IMenuBarPanel::draw() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f,0.2f,0.2f,0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f,0.3f,0.3f,0.3f));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4,2)); 

    // Horizontal buttons
    if (ImGui::Button("Open")) {
        toolbox::Asset *asset = toolbox::Acquisitor::pick_image(false);

        if (asset) {
            program::WindowState::assets.push_back(asset);
            program::WindowState::currentAsset = asset;
            program::WindowState::newAsset = true;
    
            // This will reset EVERYTHING ON THAT PANEL next time it renders
            Graphics::WindowManager::command_panel({ IImagePreviewPanel::name, RESET_ZOOM_FLAG });
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {}
    ImGui::SameLine();
    if (ImGui::Button("Settings")) {}

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
}

char *IMenuBarPanel::getFilePath() {
    return this->filepath;
}
