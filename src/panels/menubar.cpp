#include "panels/menubar.hpp"

void IMenuBarPanel::draw() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f,0.2f,0.2f,0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f,0.3f,0.3f,0.3f));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4,2)); 

    // Horizontal buttons
    if (ImGui::Button("Open")) {
        this->filepath = toolbox::Acquisitor::pick_file(false);
        toolbox::WindowManager::setChosenImagePath(this->filepath);
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
