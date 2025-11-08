#include "panels/image-preview.hpp"
#include <stdexcept>

float zoom_percentage = 0.6f;

cv::Point2d getOppositeEdge(cv::Point2d line) {
    cv::Point2d opposite;

    if (line == TOP_EDGE) {
        opposite = BOTTOM_EDGE;
    } else if (line == RIGHT_EDGE) {
        opposite = LEFT_EDGE;
    } else if (line == LEFT_EDGE) {
        opposite = RIGHT_EDGE;

    } else if (line == BOTTOM_EDGE) {
        opposite = TOP_EDGE;
    } else {
        opposite = cv::Point2d();
    }

    return opposite;
}

void IImagePreviewPanel::pre_draw() {}
bool IImagePreviewPanel::show_condition() {
    return true;
}
void IImagePreviewPanel::draw() {
    // not sure what this line is for, but I digress
    assets.push_back(assetWeak);
    this->handle_events();
    assets.pop_back();

    if (this->panel_control_flags & RESET_ZOOM_FLAG) {
        ImGui::SetScrollX(0.0f);
        ImGui::SetScrollY(0.0f);
        zoom_percentage = 0.6f;
    }
    assetWeak = Graphics::WindowManager::renderPreviewImage(zoom_percentage);

    if (pixel_inspector_mode_enabled) {
        //! Let's assume for a minute that ASSET will always exist
        // TODO: FIX IT LATER
        std::shared_ptr<toolbox::Asset> asset = assetWeak.lock();

        cv::Rect roi(current_hovered_pixel.x - 4, current_hovered_pixel.y - 4, 8, 8);
        cv::Mat preview_pixel_region = asset->displayed_image(roi);

        if (asset->displayed_image.depth() != 0) {
            throw "Non-grey scale images are not supported, EXITING...";
        }

        cv::Mat preview_scaled_region;
        cv::resize(preview_pixel_region, preview_scaled_region, cv::Size(roi.width * 16, roi.height * 16), 0, 0, cv::INTER_NEAREST);

        // SDL texture
        //TODO: Move this later to the image renderer
        cv::cvtColor(preview_scaled_region, preview_scaled_region, cv::COLOR_GRAY2BGR);
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
            preview_scaled_region.data,
            preview_scaled_region.cols,
            preview_scaled_region.rows,
            preview_scaled_region.channels() * 8,
            preview_scaled_region.step,
            SDL_PIXELFORMAT_BGR24
        );

        SDL_Texture* texture = SDL_CreateTextureFromSurface(Graphics::WindowManager::renderer, surface);

        SDL_FreeSurface(surface);

        ImGui::SetNextWindowPos(ImGui::GetIO().MousePos);
        ImGui::SetNextWindowSize(ImVec2(0, 0));

        ImVec2 image_pos = ImGui::GetCursorScreenPos();
        ImVec2 image_size = ImVec2(preview_scaled_region.cols, preview_scaled_region.rows);

        ImGui::BeginTooltip();
        ImGui::Image((ImTextureID)texture, image_size);
        ImGui::GetWindowDrawList()->AddRect(image_pos, ImVec2(image_pos.x + image_size.x, image_pos.y + image_size.y), IM_COL32(255, 255, 255, 255), 0.0f, 0, 1.0f);
        ImGui::Text("p(%d, %d): %d", current_hovered_pixel.x, current_hovered_pixel.y, current_hovered_pixel.intensity);
        ImGui::EndTooltip();
    }

    if (program::WindowState::controlsState.geoTransformEnabled) {
        // draw a rotation center circle
        std::shared_ptr<toolbox::Asset> asset = assetWeak.lock();

        if (!asset) {
            return;
        }

        ImDrawList *drawList = ImGui::GetWindowDrawList();

        float scale_x = asset->displayed_image.cols / asset->size.x;
        float scale_y = asset->displayed_image.rows / asset->size.y;

        ImVec2 absolute_coords_asset_rotation = asset->rotation_center;

        // image coords to relative coords
        float rel_x = absolute_coords_asset_rotation.x / scale_x;
        float rel_y = absolute_coords_asset_rotation.y / scale_y;

        // relative to absolute coords
        absolute_coords_asset_rotation.x = asset->position.x + rel_x;
        absolute_coords_asset_rotation.y = asset->position.y + rel_y;

        drawList->AddLine(ImVec2(absolute_coords_asset_rotation.x - 10, absolute_coords_asset_rotation.y), ImVec2(absolute_coords_asset_rotation.x + 10, absolute_coords_asset_rotation.y), IM_COL32_WHITE, 1);
        drawList->AddLine(ImVec2(absolute_coords_asset_rotation.x, absolute_coords_asset_rotation.y - 10), ImVec2(absolute_coords_asset_rotation.x, absolute_coords_asset_rotation.y + 10), IM_COL32_WHITE, 1);

        // draw a border with small squares
        // calculate the corners' real position

        ImVec2* positions = transformImageCorners(ImVec2(0, 0), asset->original_image.size, asset->transformation);
        scale_x = (asset->displayed_image.cols > 0) ? (asset->size.x / asset->displayed_image.cols) : 0;
        scale_y = (asset->displayed_image.rows > 0) ? (asset->size.y / asset->displayed_image.rows) : 0;

        for (int i = 0; i < 4; ++i) {
            positions[i].x = asset->position.x + positions[i].x * scale_x;
            positions[i].y = asset->position.y + positions[i].y * scale_y;
        }

        drawList->PushClipRect(asset->position, ImVec2(asset->position.x + asset->size.x, asset->position.y + asset->size.y), true);

        drawList->AddLine(positions[0], positions[1], IM_COL32_WHITE, 1);
        drawList->AddLine(positions[1], positions[2], IM_COL32_WHITE, 1);
        drawList->AddLine(positions[2], positions[3], IM_COL32_WHITE, 1);
        drawList->AddLine(positions[3], positions[0], IM_COL32_WHITE, 1);

        ImVec2 edge_center;
        float rect_size = 5.0f;
        // handles_abs_positions is a fixed-size array of 8 ImVec2, representing the positions of all 
        // handles on an image

        // top edge
        edge_center.x = (positions[0].x + positions[1].x) * 0.5f;
        edge_center.y = (positions[0].y + positions[1].y) * 0.5f;
        handles_abs_positions[0] = edge_center;
        drawList->AddRectFilled(ImVec2(edge_center.x - rect_size, edge_center.y - rect_size),
                    ImVec2(edge_center.x + rect_size, edge_center.y + rect_size),
                    IM_COL32_WHITE);

        // right edge
        edge_center.x = (positions[1].x + positions[2].x) * 0.5f;
        edge_center.y = (positions[1].y + positions[2].y) * 0.5f;
        handles_abs_positions[1] = edge_center;
        drawList->AddRectFilled(ImVec2(edge_center.x - rect_size, edge_center.y - rect_size),
                    ImVec2(edge_center.x + rect_size, edge_center.y + rect_size),
                    IM_COL32_WHITE);

        // bottom edge
        edge_center.x = (positions[2].x + positions[3].x) * 0.5f;
        edge_center.y = (positions[2].y + positions[3].y) * 0.5f;
        handles_abs_positions[2] = edge_center;
        drawList->AddRectFilled(ImVec2(edge_center.x - rect_size, edge_center.y - rect_size),
                    ImVec2(edge_center.x + rect_size, edge_center.y + rect_size),
                    IM_COL32_WHITE);

        // left edge
        edge_center.x = (positions[3].x + positions[0].x) * 0.5f;
        edge_center.y = (positions[3].y + positions[0].y) * 0.5f;
        handles_abs_positions[3] = edge_center;
        drawList->AddRectFilled(ImVec2(edge_center.x - rect_size, edge_center.y - rect_size),
                    ImVec2(edge_center.x + rect_size, edge_center.y + rect_size),
                    IM_COL32_WHITE);

        // draw corner squares and store corner handles
        for (int i = 0; i < 4; i++) {
            handles_abs_positions[i + 4] = positions[i];
            drawList->AddRectFilled(ImVec2(positions[i].x - rect_size, positions[i].y - rect_size),
                        ImVec2(positions[i].x + rect_size, positions[i].y + rect_size),
                        IM_COL32_WHITE);
        }

        drawList->PopClipRect();
    }
}

ImVec2* IImagePreviewPanel::transformImageCorners(ImVec2 position, cv::MatSize size,  cv::Mat transformation) {
    assert(transformation.rows == 3 && transformation.cols == 3);

    static ImVec2 positions[4];

    // top-left
    cv::Mat position_matrix = (cv::Mat_<float>(3, 1) << position.x, position.y, 1.0f);
    cv::Mat result = transformation * position_matrix;
    positions[0] = ImVec2(result.at<float>(0, 0), result.at<float>(1, 0));

    // top-right
    position_matrix = (cv::Mat_<float>(3, 1) << position.x + size[1], position.y, 1.0f);
    result = transformation * position_matrix;
    positions[1] = ImVec2(result.at<float>(0, 0), result.at<float>(1, 0));

    // bottom-right
    position_matrix = (cv::Mat_<float>(3, 1) << position.x + size[1], position.y + size[0], 1.0f);
    result = transformation * position_matrix;
    positions[2] = ImVec2(result.at<float>(0, 0), result.at<float>(1, 0));

    // bottom-left
    position_matrix = (cv::Mat_<float>(3, 1) << position.x, position.y + size[0], 1.0f);
    result = transformation * position_matrix;
    positions[3] = ImVec2(result.at<float>(0, 0), result.at<float>(1, 0));

    return positions;
}

void IImagePreviewPanel::handle_events() {
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    bool isImageHovered = ImGui::IsWindowHovered();
    bool spaceHeld = keyState[SDL_SCANCODE_SPACE];
    bool altHeld = keyState[SDL_SCANCODE_LALT];

    //! TODO: FIX THIS PIECE OF SHIT. THE PIXEL INSPECTOR WORKS RANDOMLY!
    if (isImageHovered) {
        //! This only works for one image currently!
        std::shared_ptr<toolbox::Asset> asset = assets[0].lock();

        if (asset == nullptr) {
            return;
        }

        if (altHeld) {
            pixel_inspector_mode_enabled = true;

            ImVec2 mousePos = ImGui::GetIO().MousePos;

            // relative mouse position (screen space)
            mousePos.x -= asset->position.x;
            mousePos.y -= asset->position.y;

            // scale to image space
            float scale_x = asset->displayed_image.cols / asset->size.x;
            float scale_y = asset->displayed_image.rows / asset->size.y;
            float image_x = mousePos.x * scale_x;
            float image_y = mousePos.y * scale_y;

            if (asset->displayed_image.depth() != 0) {
                throw std::runtime_error("unsupported image depth");
            }

            int ix = static_cast<int>(image_x);
            int iy = static_cast<int>(image_y);

            if (ix >= 0 && iy >= 0 && iy < asset->displayed_image.rows && ix < asset->displayed_image.cols) {
                current_hovered_pixel = {ix, iy, static_cast<int>(asset->displayed_image.at<uchar>(iy, ix))};
            }
        } else {
            pixel_inspector_mode_enabled = false;
        }

        if (spaceHeld) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            float scroll = ImGui::GetIO().MouseWheel;

            if (scroll > 0) {
                zoom_percentage *= 1.1f;
            } else if (scroll < 0 && zoom_percentage > 0.3f) {
                zoom_percentage /= 1.1f;
            }
    
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                
                ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
                ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);
    
                // forget the "consumed" drag
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }
        } else if(program::WindowState::controlsState.geoTransformFlags.rotation_center_enabled) {
            ImVec2 mouse = ImGui::GetIO().MousePos;

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
        } else if (program::WindowState::controlsState.geoTransformFlags.skew_enabled) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImGuiIO &io = ImGui::GetIO();

                ImVec2 mouse_pos = io.MousePos;
                ImVec2 mouse_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                mouse_skew_delta = ImVec2(mouse_delta.x - prev_mouse_skew_delta.x, mouse_delta.y - prev_mouse_skew_delta.y);
                prev_mouse_skew_delta = mouse_delta;

                bool found_edge = false;

                // project the mouse position on all 4 edges of the image
                for (int i = 0; i < 4; i++) {
                    short start_idx = i + 4;
                    short end_idx = ((i + 1) % 4) + 4;
                    ImVec2 start = handles_abs_positions[start_idx];
                    ImVec2 end = handles_abs_positions[end_idx];
                    // find distances
                    float distance = sqrtf(powf(end.x - start.x, 2) + powf(end.y - start.y, 2));
                    float distance_mouse_end = sqrtf(powf(end.x - mouse_pos.x, 2) + powf(end.y - mouse_pos.y, 2));
                    float distance_mouse_start = sqrtf(powf(mouse_pos.x - start.x, 2) + powf(mouse_pos.y - start.y, 2));

                    if (fabsf((distance_mouse_end + distance_mouse_start) - distance) < 3.0f) {
                        // Find the opposite edge and get its start and end
                        cv::Point2d line(start_idx, end_idx);
                        cv::Point2d opposite(getOppositeEdge(line));

                        if (opposite.x == opposite.y) {
                            return;
                        }

                        edge_held_points[0] = handles_abs_positions[(int) opposite.x];
                        edge_held_points[1] = handles_abs_positions[(int) opposite.y];
                        found_edge = true;
                        edge_held = true;
                        break;
                    }
                }

                if (found_edge || edge_held) {
                    std::shared_ptr<toolbox::Asset> asset = assetWeak.lock();

                    if (!asset) {
                        return;
                    }

                    ImDrawList *draw_list = ImGui::GetWindowDrawList();
                    draw_list->AddCircle(edge_held_points[0], 2.0f, IM_COL32(255, 0, 0, 255));
                    draw_list->AddCircle(edge_held_points[1], 2.0f, IM_COL32(255, 0, 0, 255));
                    draw_list->AddCircle(mouse_pos, 2.0f, IM_COL32(255, 0, 0, 255));
                    // Convert mouse position ( with delta transformed in mind ) and the opposite 2 edges back to image space coords
                    // relative mouse coordinates to the asset position
                    mouse_pos.x -= asset->position.x;
                    mouse_pos.y -= asset->position.y;

                    float scale_x = asset->displayed_image.cols / asset->size.x;
                    float scale_y = asset->displayed_image.rows / asset->size.y;

                    float mouse_after_rel_x = mouse_pos.x + mouse_skew_delta.x;
                    float mouse_after_rel_y = mouse_pos.y + mouse_skew_delta.y;
                    // convert opposite edge endpoints to image space
                    ImVec2 lp0 = edge_held_points[0];
                    ImVec2 lp1 = edge_held_points[1];

                    lp0.x -= asset->position.x;
                    lp0.y -= asset->position.y;
                    lp1.x -= asset->position.x;
                    lp1.y -= asset->position.y;

                    // points before
                    cv::Point2f lp0_image(lp0.x * scale_x, lp0.y * scale_y);
                    cv::Point2f lp1_image(lp1.x * scale_x, lp1.y * scale_y);
                    cv::Point2f mouse_image(mouse_pos.x * scale_x, mouse_pos.y * scale_y);

                    // points after (only mouse point is a new after-point)
                    cv::Point2f mouse_image_after(mouse_after_rel_x * scale_x, mouse_after_rel_y * scale_y);
                    // Do a skew transformation
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Skew>(*asset, 
                        lp0_image, lp1_image, mouse_image, lp0_image, lp1_image, mouse_image_after
                    );
                }
            } else {
                // reset edge held
                edge_held = false;
            }
        } else if (program::WindowState::controlsState.geoTransformFlags.scale_enabled) {
            const Uint8 *keyState = SDL_GetKeyboardState(NULL);
            bool xHeld = keyState[SDL_SCANCODE_X];
            bool yHeld = keyState[SDL_SCANCODE_Y];
            bool ctrlHeld = keyState[SDL_SCANCODE_RCTRL] || keyState[SDL_SCANCODE_LCTRL];

            if (ctrlHeld) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
            } else if (xHeld) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            } else if (yHeld) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            } else {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            }

            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                // TODO: If prefereable, just set this outside as an internal state instead of locking that every GODDAMN TIME!
                std::shared_ptr<toolbox::Asset> asset = assetWeak.lock();
                
                if (!asset) {
                    return;
                }

                ImVec2 mouse_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

                mouse_skew_delta = ImVec2(mouse_delta.x - prev_mouse_skew_delta.x, mouse_delta.y - prev_mouse_skew_delta.y);
                prev_mouse_skew_delta = mouse_delta;

                if (mouse_skew_delta.x == 0 && mouse_skew_delta.y == 0) {
                    return;
                }

                // Change to constants added from a constants file somewhere
                // TODO: Store these somewhere on global state so that the tools file can use these
                float scale_x = 1.0f + mouse_skew_delta.x * 0.01f;
                float scale_y = 1.0f + mouse_skew_delta.y * 0.01f;

                if (ctrlHeld) {
                    float avg_scale = (scale_x + scale_y) / 2.0f;
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(
                        *asset,
                        avg_scale,
                        avg_scale
                    );
                } else if (xHeld) {
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(
                        *asset,
                        scale_x,
                        1
                    );
                } else if (yHeld) {
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(
                        *asset,
                        1,
                        scale_y
                    );
                } else {
                }
            }
        } else {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
    }
}
