#include "panels/image-preview.hpp"

float zoom_percentage = 0.6f;


void IImagePreviewPanel::pre_draw() {}
bool IImagePreviewPanel::show_condition() {
    return true;
}
void IImagePreviewPanel::draw() {
    if (this->panel_control_flags & RESET_ZOOM_FLAG) {
        ImGui::SetScrollX(0.0f);
        ImGui::SetScrollY(0.0f);
        zoom_percentage = 0.6f;
    }
    std::weak_ptr<toolbox::Asset> asset = Graphics::WindowManager::renderPreviewImage(zoom_percentage);

    assets.push_back(asset);
    this->handle_events();
    assets.pop_back();
}

void IImagePreviewPanel::handle_events() {
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    bool isImageHovered = ImGui::IsWindowHovered();
    bool spaceHeld = keyState[SDL_SCANCODE_SPACE];

    if (isImageHovered) {
        float scroll = ImGui::GetIO().MouseWheel;

        if (scroll > 0) {
            zoom_percentage *= 1.1f;
        } else if (scroll < 0 && zoom_percentage > 0.3f) {
            zoom_percentage /= 1.1f;
        }

        //! This only works for one image currently!
        if(program::WindowState::controlsState.geoTransformFlags.rotation_center_enabled) {
            ImVec2 mouse = ImGui::GetIO().MousePos;
            std::shared_ptr<toolbox::Asset> asset = assets[0].lock();

            if (asset == nullptr) {
                return;
            }

            // relative mouse position (screen space)
            mouse.x -= asset->position.x;
            mouse.y -= asset->position.y;

            // scale to image space
            float scale_x = asset->displayed_image.cols / asset->size.x;
            float scale_y = asset->displayed_image.rows / asset->size.y;
            float image_x = mouse.x * scale_x;
            float image_y = mouse.y * scale_y;

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                // set rotation center in image coordinates
                free_rotate_switch = false;
                asset->rotation_center.x = image_x;
                asset->rotation_center.y = image_y;
            }
            
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                // start free rotation mode
                free_rotate_switch = true;
                float dx = image_x - asset->rotation_center.x;
                float dy = image_y - asset->rotation_center.y;
                prev_rotation_angle = atan2f(-dy, dx) * (180.0f / M_PI);
            }
            
            if (free_rotate_switch && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
                float dx = image_x - asset->rotation_center.x;
                float dy = image_y - asset->rotation_center.y;
                float current_angle = atan2f(-dy, dx) * (180.0f / M_PI);

                float angle_delta = current_angle - prev_rotation_angle;

                if (angle_delta > 180.0f) {
                    angle_delta -= 360.0f;
                } else if (angle_delta < -180.0f) {
                    angle_delta += 360.0f;
                }

                prev_rotation_angle = current_angle;
                asset->rotation_angle +=  angle_delta;

                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Rotation>(
                    *asset,
                    asset->rotation_center.x,
                    asset->rotation_center.y,
                    angle_delta
                );
            } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                free_rotate_switch = false;
                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            } else if (!free_rotate_switch) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            }
        } else {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
    } else {
        if (isImageHovered && spaceHeld) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                
                ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
                ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);
    
                // forget the "consumed" drag
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }
        } else {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
    }

}
