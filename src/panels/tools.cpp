#include "panels/tools.hpp"

void IToolsPanel::pre_draw() {
    // set internal state
    assetPtr = acquisitor->getLatestAsset();
}

void IToolsPanel::handle_events() {
    if (buttonsPressed) {
        std::shared_ptr<toolbox::Asset> asset = assetPtr.lock();

        if (buttonPressed(EQUALIZE_BUTTON)) {
            if (asset) {
                toolbox::OpenCVProcessor::process<toolbox::HistogramEqualize>(*asset);
            }

            buttonsPressed &= ~(1 << EQUALIZE_BUTTON);
        }

        if (buttonPressed(GEO_TRANSFORMATION_BUTTON)) {
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

            if (translation[0] || translation[1]) {
                if (asset) {
                    toolbox::OpenCVProcessor::process<toolbox::GeometricTransformation::Translate>(*asset, translation[0], translation[1]);
                }
            }
        }
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

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // Transparent
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.9f, 0.4f)); // Bluish
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.3f, 0.6f, 0.9f, 0.6f)); // Slightly more opaque blue
    ImGui::PushStyleColor(ImGuiCol_Border,        ImVec4(0.8f, 0.8f, 0.8f, 0.5f)); // Light gray border

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

    buttonsPressed |= ImGui::Button(ICON_FA_CHART_LINE, ImVec2(this->size.x - 20, this->size.x - 20)) << EQUALIZE_BUTTON;

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Add 10 pixels of vertical space between buttons

    if (popup_geo_transform_open) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 0.4f));
    }

    bool geoPressed = ImGui::Button(ICON_FA_VECTOR_SQUARE, ImVec2(this->size.x - 20, this->size.x - 20));
    float max_transformation_x = (float) asset->displayed_image.cols;
    float max_transformation_y = (float) asset->displayed_image.rows;

    if (geoPressed) {
        // prev status xor 1 = toggle on off
        buttonsPressed ^= 1 << GEO_TRANSFORMATION_BUTTON;
    }

    if (popup_geo_transform_open) {
        ImGui::PopStyleColor();
    }

    if (buttonPressed(GEO_TRANSFORMATION_BUTTON) && !popup_geo_transform_open) {
        ImGui::OpenPopup("GEO_POPUP");
        popup_geo_transform_open = true;
    } else if (!buttonPressed(GEO_TRANSFORMATION_BUTTON)) {
        popup_geo_transform_open = false;
    }

    ImVec2 button_top_right = ImVec2(ImGui::GetItemRectMax().x + 20, ImGui::GetItemRectMin().y);
    
    // Set the popup's top-left corner to be at the button's top-right
    ImGui::SetNextWindowPos(button_top_right);

    if (ImGui::BeginPopup("GEO_POPUP")) {
        ImGui::SeparatorText("Geometric Transformations");
        ImGui::Text("Translate");
        slider_transform_x_changed = ImGui::SliderFloat("X-Axis", &transformation_x, -max_transformation_x, max_transformation_x);
        slider_transform_y_changed = ImGui::SliderFloat("Y-Axis", &transformation_y, -max_transformation_y, max_transformation_y);

        if (!buttonPressed(GEO_TRANSFORMATION_BUTTON)) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    } else {
        // reset the popup state
        popup_geo_transform_open = false;
        buttonsPressed &= ~(1 << GEO_TRANSFORMATION_BUTTON);
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 textPos = ImVec2((this->position.x + this->size.x) / 8, this->position.y + this->size.y - 30);
    draw_list->AddText(textPos, IM_COL32(255,255,255, 255), "Tools");
    draw_list->AddLine(ImVec2(this->position.x + 10, textPos.y - 15), ImVec2(this->position.x + this->size.x - 10, textPos.y - 15), IM_COL32(255, 255, 255, 80));


    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    this->handle_events();
}
