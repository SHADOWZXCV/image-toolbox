#include "panels/tools.hpp"

void IToolsPanel::pre_draw() {
    // set internal state
    assetPtr = acquisitor->getLatestAsset();

    std::shared_ptr<toolbox::Asset> asset = assetPtr.lock();

    if (asset == nullptr) {
        return;
    }

    rotate_center[0] = asset->rotation_center.x;
    rotate_center[1] = asset->rotation_center.y;
    rotate_angle = asset->rotation_angle;
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
                slider_transform_x_changed = false;
            }
            if (slider_transform_y_changed) {
                float y_val = transformation_y - prev_transformation_y;
                prev_transformation_y = transformation_y;
                translation[1] = y_val;
                slider_transform_y_changed = false;
            }

            if (rotate_center_changed || rotate_angle_changed) {
                asset->rotation_center.x = rotate_center[0];
                asset->rotation_center.y = rotate_center[1];
                asset->rotation_angle = rotate_angle - prev_rotate_angle;
                prev_rotate_angle = rotate_angle;

                if (asset) {
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Rotation>(
                        *asset,
                        asset->rotation_center.x,
                        asset->rotation_center.y,
                        asset->rotation_angle
                    );
                }

                rotate_center_changed = false;
                rotate_angle_changed = false;
            }

            if (translation[0] || translation[1]) {
                if (asset) {
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Translate>(*asset, translation[0], translation[1]);
                }
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

                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(*asset, delta_scale_x, delta_scale_y);
            } else if (scale_same_ratio_slider_changed) {
                float delta_scale = prev_scale_same_ratio ? scale_same_ratio / prev_scale_same_ratio : 1;

                prev_scale_same_ratio = scale_same_ratio;
                toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Scale>(*asset, delta_scale, delta_scale);
            }

            program::WindowState::controlsState.geoTransformFlags.scale_enabled = scale_mouse_checked;
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

    buttonsPressed |= ImGui::Button(ICON_FA_CHART_LINE, ImVec2(this->size.x - 20, this->size.x - 20)) << EQUALIZE_BUTTON;

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    bool wasContrastPressed = buttonPressed(CONTRAST_BUTTON);

    if (wasContrastPressed) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 0.4f));
    }

    buttonsPressed ^= ImGui::Button(ICON_FA_CIRCLE_HALF_STROKE, ImVec2(this->size.x - 20, this->size.x - 20)) << CONTRAST_BUTTON;


    if (wasContrastPressed) {
        ImGui::PopStyleColor();

        ImVec2 button_top_right = ImVec2(ImGui::GetItemRectMax().x + 20, ImGui::GetItemRectMin().y);
    
        // Set the popup's top-left corner to be at the button's top-right
        ImGui::SetNextWindowPos(button_top_right);
        ImGui::SetNextWindowSize(ImVec2(0, 0));
        ImGui::Begin("CONTRAST_POPUP", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SeparatorText("Contrast stretching options");

        ImGui::InputScalar("R1", ImGuiDataType_U8, &contrast_r1);
        ImGui::InputScalar("R2", ImGuiDataType_U8, &contrast_r2);
        ImGui::InputScalar("S1", ImGuiDataType_U8, &contrast_s1);
        ImGui::InputScalar("S2", ImGuiDataType_U8, &contrast_s2);

        contrast_stretch_pressed = ImGui::Button("Apply");
        
        ImGui::End();
    }

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    bool wasGeoPressed = buttonPressed(GEO_TRANSFORMATION_BUTTON);

    if (wasGeoPressed) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 0.4f));
    }

    buttonsPressed ^= ImGui::Button(ICON_FA_VECTOR_SQUARE, ImVec2(this->size.x - 20, this->size.x - 20)) << GEO_TRANSFORMATION_BUTTON;

    float delta_scale_x = prev_scale_x ? scale_x / prev_scale_x : 1;
    float delta_scale_y = prev_scale_y ? scale_y / prev_scale_y : 1;
    float max_transformation_x = (float) asset->displayed_image.cols * delta_scale_x;
    float max_transformation_y = (float) asset->displayed_image.rows * delta_scale_y;

    if (wasGeoPressed) {
        ImGui::PopStyleColor();
    }

    if (wasGeoPressed) {
        ImVec2 button_top_right = ImVec2(ImGui::GetItemRectMax().x + 20, ImGui::GetItemRectMin().y);
    
        // Set the popup's top-left corner to be at the button's top-right
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

        // can be converted to a binary masker instead of all these if conditions, but later :/
        if (mouseSkewPressed) {
            rotate_center_mouse_checked = false;
            scale_mouse_checked = false;
        } else if (mouseRotatePressed) {
            skew_checked = false;
            scale_mouse_checked = false;
        } else if (mouseScalePressed) {
            rotate_center_mouse_checked = false;
            skew_checked = false;
        }

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
