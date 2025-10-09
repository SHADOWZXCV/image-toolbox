#include "panels/image-preview.hpp"

float zoom_percentage = 0.6f;

void IImagePreviewPanel::draw() {
    if (this->panel_control_flags & RESET_ZOOM_FLAG) {
        ImGui::SetScrollX(0.0f);
        ImGui::SetScrollY(0.0f);
        zoom_percentage = 0.6f;
    }
    Graphics::WindowManager::renderPreviewImage(zoom_percentage);
    this->handle_events();
}

void IImagePreviewPanel::handle_events() {
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    bool isPanelHovered = ImGui::IsWindowHovered();
    bool spaceHeld = keyState[SDL_SCANCODE_SPACE];

    if (isPanelHovered) {
        float scroll = ImGui::GetIO().MouseWheel;

        if (scroll > 0) {
            zoom_percentage += 0.05f;
        } else if (scroll < 0 && zoom_percentage > 0.3f) {
            zoom_percentage -= 0.05f;
        }
    }

    if (isPanelHovered && spaceHeld) {
        SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(cursor);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            
            ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
            ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);

            // forget the "consumed" drag
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
    } else {
        SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_SetCursor(cursor);
    }
}
