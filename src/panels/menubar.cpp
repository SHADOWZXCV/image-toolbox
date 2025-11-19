#include "panels/menubar.hpp"
#include "shared/theme.hpp"

void IMenuBarPanel::pre_draw() {}
bool IMenuBarPanel::show_condition() {
    return true;
}
void IMenuBarPanel::handle_events() {}

void IMenuBarPanel::draw() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4,2)); 

    // Horizontal buttons
    if (ImGui::Button("Open")) {
        std::shared_ptr<toolbox::Asset> asset = toolbox::Acquisitor::pick_image(false);

        if (asset) {
            program::setChosenAsset(asset);
            // This will reset EVERYTHING ON THAT PANEL next time it renders
            Graphics::WindowManager::command_panel({ IImagePreviewPanel::name, RESET_ZOOM_FLAG });
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {}
    ImGui::SameLine();
    if (ImGui::Button("Settings")) {}

    ImGui::SameLine();
    bool themeBtnPressed = ImGui::Button("Theme");
    bool themeBtnHovered = ImGui::IsItemHovered();

    if (themeBtnHovered || themeBtnPressed) {
        ImVec2 btnMin = ImGui::GetItemRectMin();
        ImVec2 btnMax = ImGui::GetItemRectMax();
        ImVec2 popupPos(btnMax.x + 8.0f, btnMin.y);
        ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);
        ImGui::OpenPopup("THEME_POPUP");
    }

    if (ImGui::BeginPopup("THEME_POPUP")) {
        if (ImGui::Selectable("Light")) { 
            theme::ApplyLightTheme(); 
        }
        if (ImGui::Selectable("Dark"))  { 
            theme::ApplyDarkTheme();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
}

char *IMenuBarPanel::getFilePath() {
    return this->filepath;
}
