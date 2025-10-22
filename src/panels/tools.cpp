#include "panels/tools.hpp"

void IToolsPanel::pre_draw() {}
void IToolsPanel::handle_events() {
    if (buttonsPressed & (1 << EQUALIZE_BUTTON)) {
        program::IAcquisitorService* acquisitor = static_cast<program::IAcquisitorService*>(program::ServiceManager::getByName("acquisitor_service"));
        toolbox::OpenCVProcessor::process<toolbox::HistogramEqualize>(acquisitor->getLatestAsset()->image);
        program::WindowState::textureUpdate = true;
    }
}

bool IToolsPanel::show_condition() {
    // TODO: Reserve names in an internal state
    program::IAcquisitorService* acquisitor = static_cast<program::IAcquisitorService*>(program::ServiceManager::getByName("acquisitor_service"));
    std::vector<toolbox::Asset *> *assets = acquisitor->getAssets();

    if (assets->empty()) {
        return false;
    }

    return true;
}

void IToolsPanel::draw() {
    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Transparent
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 0.4f)); // Bluish
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.3f, 0.6f, 0.9f, 0.6f)); // Slightly more opaque blue
    ImGui::PushStyleColor(ImGuiCol_Border,        ImVec4(0.8f, 0.8f, 0.8f, 0.5f)); // Light gray border

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    buttonsPressed = 0;
    buttonsPressed |= ImGui::Button(ICON_FA_CHART_LINE, ImVec2(this->size.x - 20, this->size.x - 20)) << EQUALIZE_BUTTON;

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Add 10 pixels of vertical space between buttons
    ImGui::Button(ICON_FA_FIRE_FLAME_CURVED, ImVec2(this->size.x - 20, this->size.x - 20));

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 textPos = ImVec2((this->position.x + this->size.x) / 8, this->position.y + this->size.y - 30);
    draw_list->AddText(textPos, IM_COL32(255,255,255, 255), "Tools");
    draw_list->AddLine(ImVec2(this->position.x + 10, textPos.y - 15), ImVec2(this->position.x + this->size.x - 10, textPos.y - 15), IM_COL32(255, 255, 255, 80));


    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    this->handle_events();
}
