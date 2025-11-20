#include "panels/tools.hpp"
#include <cmath>

void IToolsPanel::pre_draw() {
    // set internal state
    assetPtr = acquisitor->getLatestAsset();

    std::shared_ptr<toolbox::Asset> asset = assetPtr.lock();

    if (asset == nullptr) {
        return;
    }

    // Sync UI state with current asset transformation
    rotate_center[0] = asset->rotation_center.x;
    rotate_center[1] = asset->rotation_center.y;

    // Use persistent components (avoid decomposition drift)
    transformation_x = prev_transformation_x = asset->translation.x;
    transformation_y = prev_transformation_y = asset->translation.y;
    scale_x = prev_scale_x = asset->scale_factors.x <= 0 ? 1.0f : asset->scale_factors.x;
    scale_y = prev_scale_y = asset->scale_factors.y <= 0 ? 1.0f : asset->scale_factors.y;
    scale_same_ratio = prev_scale_same_ratio = (scale_aspect_ratio_checked ? (scale_x + scale_y) * 0.5f : scale_same_ratio);
    rotate_angle = prev_rotate_angle = asset->rotation_angle;
    // Sync point processing UI from asset snapshot params ONLY when popup not open
    // This allows user to adjust sliders without them being reset every frame.
    if (!buttonPressed(POINT_PROCESSING_BUTTON)) {
        point_gamma = asset->pointParams.gamma;
        point_slice_min = asset->pointParams.slice_min;
        point_slice_max = asset->pointParams.slice_max;
        point_bit_preview_idx = asset->pointParams.bit_plane;
        point_slice_preserve = asset->pointParams.slice_preserve;
        slice_use_constant = asset->pointParams.slice_constant;
        slice_constant_value = asset->pointParams.slice_constant_value;
    }
}

void IToolsPanel::handle_events() {
    if (buttonsPressed) {
        std::shared_ptr<toolbox::Asset> asset = assetPtr.lock();

        if (buttonPressed(EQUALIZE_BUTTON)) {
            if (asset) {
                toolbox::OpenCVProcessor::process<toolbox::Enahnce::HistogramEqualize>(*asset);
            }

            buttonsPressed &= ~(1 << EQUALIZE_BUTTON);
        }

        if (buttonPressed(CONTRAST_BUTTON)) {
            if (contrast_stretch_pressed) {
                if (asset) {
                    toolbox::OpenCVProcessor::process<toolbox::Enahnce::ContrastStretch>(*asset, contrast_r1, contrast_r2, contrast_s1, contrast_s2);
                }

                contrast_stretch_pressed = false;
            }
        }

        if (buttonPressed(GEO_TRANSFORMATION_BUTTON)) {
            program::WindowState::controlsState.geoTransformEnabled = true;
            float translation[2] = {0, 0};

            if (slider_transform_x_changed) {
                float x_val = transformation_x - prev_transformation_x;
                prev_transformation_x = transformation_x;
                translation[0] = x_val;
                asset->translation.x += x_val;
                slider_transform_x_changed = false;
            }
            if (slider_transform_y_changed) {
                float y_val = transformation_y - prev_transformation_y;
                prev_transformation_y = transformation_y;
                translation[1] = y_val;
                asset->translation.y += y_val;
                slider_transform_y_changed = false;
            }

            if (rotate_center_changed || rotate_angle_changed) {
                asset->rotation_center.x = rotate_center[0];
                asset->rotation_center.y = rotate_center[1];
                float delta_angle = rotate_angle - prev_rotate_angle;
                prev_rotate_angle = rotate_angle;
                asset->rotation_angle = rotate_angle;
                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Rotation>(
                    *asset,
                    asset->rotation_center.x,
                    asset->rotation_center.y,
                    delta_angle
                );
                rotate_center_changed = false;
                rotate_angle_changed = false;
            }

            if (translation[0] || translation[1]) {
                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Translate>(*asset, translation[0], translation[1]);
            }

            program::WindowState::controlsState.geoTransformFlags.rotation_center_enabled = rotate_center_mouse_checked;
            program::WindowState::controlsState.geoTransformFlags.skew_enabled = skew_checked;
            // useless atm, but if I need it later I can fix it
            program::WindowState::controlsState.geoTransformFlags.flip_command_enabled = flip_x_pressed || flip_y_pressed;

            if (flip_x_pressed || flip_y_pressed) {
                program::WindowState::controlsState.geoTransform.flip = {flip_x_pressed, flip_y_pressed};
                program::BoolVec2 flip = {flip_x_pressed, flip_y_pressed};
                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Flip>(*asset, flip.x, flip.y);

                program::WindowState::controlsState.geoTransformFlags.flip_command_enabled = false;
                flip_x_pressed = flip_y_pressed = false;
            } else {
                program::WindowState::controlsState.geoTransform.flip = {0, 0};
            }

            // check for scaling
            if (scale_x_slider_changed || scale_y_slider_changed) {
                float delta_scale_x = prev_scale_x ? scale_x / prev_scale_x : 1;
                float delta_scale_y = prev_scale_y ? scale_y / prev_scale_y : 1;
                prev_scale_x = scale_x;
                prev_scale_y = scale_y;
                asset->scale_factors.x = scale_x;
                asset->scale_factors.y = scale_y;
                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(*asset, delta_scale_x, delta_scale_y);
            } else if (scale_same_ratio_slider_changed) {
                float delta_scale = prev_scale_same_ratio ? scale_same_ratio / prev_scale_same_ratio : 1;
                prev_scale_same_ratio = scale_same_ratio;
                asset->scale_factors.x = scale_same_ratio;
                asset->scale_factors.y = scale_same_ratio;
                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(*asset, delta_scale, delta_scale);
            }

            program::WindowState::controlsState.geoTransformFlags.scale_enabled = scale_mouse_checked;
            program::WindowState::controlsState.geoTransformFlags.translate_enabled = translate_mouse_checked;
        }
        // Point processing events
        if (buttonPressed(POINT_PROCESSING_BUTTON) && asset) {
            if (point_power_pressed) {
                asset->pointParams.has_power = true;
                asset->pointParams.gamma = point_gamma;
                asset->pointParams.log_used = false;
                toolbox::OpenCVProcessor::process<toolbox::PointProcessing::PowerLaw>(*asset, point_gamma);
                point_power_pressed = false;
            }
            if (point_log_pressed) {
                asset->pointParams.log_used = true;
                asset->pointParams.has_power = false;
                toolbox::OpenCVProcessor::process<toolbox::PointProcessing::LogTransform>(*asset);
                point_log_pressed = false;
            }
            if (point_invert_pressed) {
                asset->pointParams.invert_used = true;
                toolbox::OpenCVProcessor::process<toolbox::PointProcessing::Inversion>(*asset);
                point_invert_pressed = false;
            }
            if (point_slice_pressed) {
                asset->pointParams.has_slice = true;
                asset->pointParams.slice_min = point_slice_min;
                asset->pointParams.slice_max = point_slice_max;
                asset->pointParams.slice_preserve = point_slice_preserve;
                asset->pointParams.slice_constant = slice_use_constant;
                asset->pointParams.slice_constant_value = slice_constant_value;
                toolbox::OpenCVProcessor::process<toolbox::PointProcessing::GrayLevelSlice>(*asset, point_slice_min, point_slice_max, point_slice_preserve, slice_use_constant, slice_constant_value);
                point_slice_pressed = false;
            }
            if (point_bitplane_pressed) {
                asset->pointParams.bit_plane_used = true;
                asset->pointParams.bit_plane = point_bit_preview_idx;
                unsigned int mask = (1u << point_bit_preview_idx);
                toolbox::OpenCVProcessor::process<toolbox::PointProcessing::BitPlaneSlice>(*asset, mask);
                point_bitplane_pressed = false;
            }
        }
    }

    if (!buttonPressed(GEO_TRANSFORMATION_BUTTON)) {
        program::WindowState::controlsState.geoTransformEnabled = false;
    }
}

bool IToolsPanel::buttonPressed(enum ToolsButtons BUTTON) {
    return buttonsPressed & (1 << BUTTON);
}

bool IToolsPanel::show_condition() {
    // TODO: Reserve names in an internal state
    std::vector<std::weak_ptr<toolbox::Asset>> *assets = acquisitor->getAssets();

    if (assets->empty()) {
        return false;
    }

    return true;
}

void IToolsPanel::draw() {
    std::shared_ptr<toolbox::Asset> asset = assetPtr.lock();

    if(asset == nullptr)
        return;

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.3f, 0.6f, 0.9f, 0.6f));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    // Equalize (no persistent popup)
    if (ImGui::Button(ICON_FA_CHART_LINE, ImVec2(this->size.x - 20, this->size.x - 20))) {
        buttonsPressed |= (1u << EQUALIZE_BUTTON);
    }
    ImGui::Dummy(ImVec2(0,5));
    // Centralized popup buttons
    auto popupButton = [&](enum ToolsButtons btn, const char* icon){
        bool active = (activePopup == (int)btn);
        ImVec4 baseCol = ImVec4(0.0f,0.0f,0.0f,0.0f);
        ImVec4 actCol  = ImVec4(0.3f,0.6f,0.9f,0.4f);
        ImGui::PushStyleColor(ImGuiCol_Button, active?actCol:baseCol);
        bool clicked = ImGui::Button(icon, ImVec2(this->size.x - 20, this->size.x - 20));
        ImGui::PopStyleColor();
        // store rect for popup anchor
        buttonMin[btn] = ImGui::GetItemRectMin();
        buttonMax[btn] = ImGui::GetItemRectMax();
        if (clicked) {
            if (active) {
                activePopup = -1;
                buttonsPressed &= ~(1u << btn);
            } else {
                buttonsPressed &= ~((1u << CONTRAST_BUTTON)|(1u<<GEO_TRANSFORMATION_BUTTON)|(1u<<POINT_PROCESSING_BUTTON));
                activePopup = (int)btn;
                buttonsPressed |= (1u << btn);
            }
        }
        ImGui::Dummy(ImVec2(0,5));
    };
    popupButton(CONTRAST_BUTTON, ICON_FA_CIRCLE_HALF_STROKE);
    popupButton(GEO_TRANSFORMATION_BUTTON, ICON_FA_VECTOR_SQUARE);
    popupButton(POINT_PROCESSING_BUTTON, ICON_FA_BOLT);

    bool wasContrastPressed = (activePopup == CONTRAST_BUTTON);
    bool wasGeoPressed = (activePopup == GEO_TRANSFORMATION_BUTTON);
    bool wasPointPressed = (activePopup == POINT_PROCESSING_BUTTON);

    float delta_scale_x = prev_scale_x ? scale_x / prev_scale_x : 1;
    float delta_scale_y = prev_scale_y ? scale_y / prev_scale_y : 1;
    float max_transformation_x = (float) asset->displayed_image.cols * delta_scale_x;
    float max_transformation_y = (float) asset->displayed_image.rows * delta_scale_y;

    if (wasGeoPressed) {
        ImVec2 anchor = buttonMax[GEO_TRANSFORMATION_BUTTON];
        ImVec2 button_top_right = ImVec2(anchor.x + 20, buttonMin[GEO_TRANSFORMATION_BUTTON].y);
        ImGui::SetNextWindowPos(button_top_right);
        ImGui::SetNextWindowSize(ImVec2(0, 0));
        ImGui::Begin("GEO_POPUP", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SeparatorText("Geometric Transformations");

        ImGui::Text("Translate");
        slider_transform_x_changed = ImGui::SliderFloat("X-Axis", &transformation_x, -max_transformation_x, max_transformation_x);
        slider_transform_y_changed = ImGui::SliderFloat("Y-Axis", &transformation_y, -max_transformation_y, max_transformation_y);

        ImGui::Separator();
        ImGui::Text("Rotate");

        bool mouseRotatePressed = ImGui::Checkbox("Mouse-select center", &rotate_center_mouse_checked);

        ImGui::BeginDisabled(rotate_center_mouse_checked);

        rotate_center_changed = ImGui::InputFloat2("Center of rotation (x, y)", rotate_center);
        rotate_angle_changed = ImGui::SliderFloat("Rotation angle in degrees", &rotate_angle, 0, 360);

        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::Text("Skew");
        bool mouseSkewPressed = ImGui::Checkbox("Enable Skew", &skew_checked);

        ImGui::Separator();
        ImGui::Text("Flip");

        bool flipXPressed = ImGui::Button("Flip Horizontally");

        ImGui::SameLine();

        bool flipYPressed = ImGui::Button("Flip Vertically");

        if (flipXPressed) {
            flip_x_pressed = true;
        }

        if (flipYPressed) {
            flip_y_pressed = true;
        }

        ImGui::Separator();
        ImGui::Text("Scale");

        
        ImGui::BeginDisabled(scale_mouse_checked);
        ImGui::Checkbox("Maintain aspect ratio", &scale_aspect_ratio_checked);

        if (scale_aspect_ratio_checked) {
            scale_same_ratio_slider_changed = ImGui::SliderFloat("Scale", &scale_same_ratio, 0.1f, 10.0f);
            scale_x = scale_same_ratio;
            scale_y = scale_same_ratio;
            prev_scale_x = scale_same_ratio;
            prev_scale_y = scale_same_ratio;
        } else {
            scale_x_slider_changed = ImGui::SliderFloat("Scale X-Axis", &scale_x, 0.1f, 10.0f);
            scale_y_slider_changed = ImGui::SliderFloat("Scale Y-Axis", &scale_y, 0.1f, 10.0f);
        }

        ImGui::EndDisabled();

        bool mouseScalePressed = ImGui::Checkbox("Mouse Scale", &scale_mouse_checked);
        bool mouseTranslatePressed = ImGui::Checkbox("Mouse Translate", &translate_mouse_checked);

        // can be converted to a binary masker instead of all these if conditions, but later :/
        if (mouseSkewPressed) {
            rotate_center_mouse_checked = false;
            scale_mouse_checked = false;
            translate_mouse_checked = false;
        } else if (mouseRotatePressed) {
            skew_checked = false;
            scale_mouse_checked = false;
            translate_mouse_checked = false;
        } else if (mouseScalePressed) {
            rotate_center_mouse_checked = false;
            skew_checked = false;
            translate_mouse_checked = false;
        } else if (mouseTranslatePressed) {
            rotate_center_mouse_checked = false;
            skew_checked = false;
            scale_mouse_checked = false;
        }

        ImGui::End();
    }
    // Contrast Popup
    if (wasContrastPressed) {
        ImVec2 anchor = buttonMax[CONTRAST_BUTTON];
        ImVec2 button_top_right = ImVec2(anchor.x + 20, buttonMin[CONTRAST_BUTTON].y);
        ImGui::SetNextWindowPos(button_top_right);
        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("CONTRAST_POPUP", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SeparatorText("Contrast stretching options");
        ImGui::InputScalarNoText("R1", ImGuiDataType_U16, &contrast_r1);
        ImGui::InputScalarNoText("R2", ImGuiDataType_U16, &contrast_r2);
        ImGui::InputScalarNoText("S1", ImGuiDataType_U16, &contrast_s1);
        ImGui::InputScalarNoText("S2", ImGuiDataType_U16, &contrast_s2);
        contrast_stretch_pressed = ImGui::Button("Apply");
        ImGui::End();
    }
    // Point Processing Popup
    if (wasPointPressed) {
        ImVec2 anchor = buttonMax[POINT_PROCESSING_BUTTON];
        ImVec2 button_top_right = ImVec2(anchor.x + 20, buttonMin[POINT_PROCESSING_BUTTON].y);
        ImGui::SetNextWindowPos(button_top_right);
        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("POINT_POPUP", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SeparatorText("Point Processing");
        // Power Law
        ImGui::Text("Power Law (Gamma)");
        ImGui::SliderFloat("Gamma", &point_gamma, 0.1f, 5.0f);
        point_power_pressed = ImGui::Button("Apply Gamma");
        ImGui::Separator();
        // Log Transform
        ImGui::Text("Log Transform");
        point_log_pressed = ImGui::Button("Apply Log");
        ImGui::Separator();
        // Inversion
        ImGui::Text("Inversion");
        point_invert_pressed = ImGui::Button("Apply Invert");
        ImGui::Separator();
        ImGui::Text("Gray Level Slice");
        ImGui::SliderInt("Min", &point_slice_min, 0, 255);
        ImGui::SliderInt("Max", &point_slice_max, 0, 255);
        if (!point_slice_preserve && !slice_use_constant) {
            ImGui::Checkbox("Preserve Others", &point_slice_preserve);
            ImGui::Checkbox("Use Constant Highlight", &slice_use_constant);
            if (point_slice_preserve && slice_use_constant) {
                slice_use_constant = false; // prefer preserve
            }
        } else if (point_slice_preserve) {
            ImGui::Checkbox("Preserve Others", &point_slice_preserve);
            if (!point_slice_preserve) {
                slice_use_constant = false; // reset both
            } else {
                slice_use_constant = false; // enforce exclusivity
            }
        } else if (slice_use_constant) {
            ImGui::Checkbox("Use Constant Highlight", &slice_use_constant);
            if (!slice_use_constant) {
                point_slice_preserve = false; // reset both
            } else {
                point_slice_preserve = false; // enforce exclusivity
                ImGui::SliderInt("Constant Value", &slice_constant_value, 0, 255);
            }
        }
        point_slice_pressed = ImGui::Button("Apply Slice");
        ImGui::Separator();
        // Bit Plane Preview
        ImGui::Text("Bit Plane Preview");
        ImGui::SliderInt("Plane", &point_bit_preview_idx, 0, 7);
        // Build preview texture if needed
        std::shared_ptr<toolbox::Asset> asset = assetPtr.lock();
        if (asset) {
            int w = asset->base_image.cols;
            int h = asset->base_image.rows;
            if (point_bit_preview_tex == nullptr || point_bit_preview_w != w || point_bit_preview_h != h) {
                if (point_bit_preview_tex) { SDL_DestroyTexture(point_bit_preview_tex); point_bit_preview_tex = nullptr; }
            }
            // regenerate texture every frame for simplicity
            cv::Mat preview(h, w, CV_8UC1);
            for (int r = 0; r < h; ++r) {
                const uchar* srcRow = asset->base_image.ptr<uchar>(r);
                uchar* dstRow = preview.ptr<uchar>(r);
                for (int c = 0; c < w; ++c) {
                    uchar bit = (srcRow[c] >> point_bit_preview_idx) & 0x1;
                    dstRow[c] = bit ? 255 : 0;
                }
            }
            // convert to BGR for SDL texture
            cv::Mat preview_bgr; cv::cvtColor(preview, preview_bgr, cv::COLOR_GRAY2BGR);
            SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
                (void*)preview_bgr.data,
                preview_bgr.cols,
                preview_bgr.rows,
                24,
                (int)preview_bgr.step,
                SDL_PIXELFORMAT_BGR24
            );
            if (surface) {
                if (point_bit_preview_tex) SDL_DestroyTexture(point_bit_preview_tex);
                point_bit_preview_tex = SDL_CreateTextureFromSurface(Graphics::WindowManager::renderer, surface);
                SDL_FreeSurface(surface);
                point_bit_preview_w = w; point_bit_preview_h = h;
            }
            if (point_bit_preview_tex) {
                // scale down preview if large
                float maxPreview = 128.0f;
                float scale = std::min(maxPreview / w, maxPreview / h);
                ImVec2 pvSize(w * scale, h * scale);
                ImGui::Image((ImTextureID)point_bit_preview_tex, pvSize);
            } else {
                ImGui::Text("Preview unavailable");
            }
        } else {
            ImGui::Text("No asset for preview");
        }
        point_bitplane_pressed = ImGui::Button("Apply Plane");
        ImGui::End();
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 textPos = ImVec2((this->position.x + this->size.x) / 8, this->position.y + this->size.y - 30);
    ImGuiStyle& style = ImGui::GetStyle();
    ImU32 textCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);
    ImU32 sepCol  = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Separator]);
    draw_list->AddText(textPos, textCol, "Tools");
    draw_list->AddLine(ImVec2(this->position.x + 10, textPos.y - 15), ImVec2(this->position.x + this->size.x - 10, textPos.y - 15), sepCol);

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    this->handle_events();
}
