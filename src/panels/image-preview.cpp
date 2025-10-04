#include "panels/image-preview.hpp"

void IImagePreviewPanel::draw() {
    float width = ImGui::GetWindowSize().x;
    float height = ImGui::GetWindowSize().y;
    ImVec2 position = ImVec2(width / 5, height / 5);
    ImGui::SetCursorPos(position);
    Graphics::WindowManager::renderPreviewImage();
}
