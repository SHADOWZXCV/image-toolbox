#pragma once
#include "fontawesome/IconsFontAwesome6.h"
#include "imgui_internal.h"
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "services/acquisitor.hpp"
#include "acquisitor/acquisitor.hpp"
#include "panels/image-preview.hpp"
#include "renderer/processor.hpp"
#include "renderer/transformations/enhance.hpp"
#include "renderer/transformations/geometric.hpp"
#include "renderer/transformations/point.hpp"

enum ToolsButtons {
    EQUALIZE_BUTTON = 0,
    CONTRAST_BUTTON = 1,
    GEO_TRANSFORMATION_BUTTON = 2,
    POINT_PROCESSING_BUTTON = 3
};

struct IToolsPanel : public IPanel {
    unsigned int getID() const override { return id; }
    std::string getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void pre_draw() override;
    void draw() override;
    void handle_events() override;
    bool show_condition() override;

    bool buttonPressed(enum ToolsButtons BUTTON);

    static constexpr const char* name = "Tools";
    private:
        // basic properties
        ImVec2 position{(float) 0, 30};
        ImVec2 size{50, (float) program::DISPLAY_HEIGHT - 30};
        ImGuiWindowFlags imGuiFlags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoResize;
    
        // state
        uint32_t buttonsPressed = 0;
        int activePopup = -1; // -1 none, else ToolsButtons value
        ImVec2 buttonMin[4]{}; // store last frame button bounds for positioning popups
        ImVec2 buttonMax[4]{};
        float transformation_x = 0.0f;
        float transformation_y = 0.0f;
        float prev_transformation_x = 0.0f;
        float prev_transformation_y = 0.0f;
        float rotate_center[2] = { 0, 0 };
        float rotate_angle = 0;
        float prev_rotate_angle = 0;
        float scale_x = 1.0f;
        float scale_y = 1.0f;
        float scale_same_ratio = 1.0f;
        float prev_scale_x = 1.0f;
        float prev_scale_y = 1.0f;
        short contrast_r1 = 0;
        short contrast_r2 = 0;
        short contrast_s1 = 0;
        short contrast_s2 = 0;
        float prev_scale_same_ratio = 1.0f;
        bool scale_mouse_checked = false;
        bool translate_mouse_checked = false;
        bool scale_x_slider_changed = false;
        bool scale_y_slider_changed = false;
        bool scale_same_ratio_slider_changed = false;
        bool rotate_center_mouse_checked = false;
        bool slider_transform_x_changed = false;
        bool slider_transform_y_changed = false;
        bool rotate_center_changed = false;
        bool rotate_angle_changed = false;
        bool skew_checked = false;
        bool flip_x_pressed = false;
        bool flip_y_pressed = false;
        bool scale_aspect_ratio_checked = false;
        bool contrast_stretch_pressed = false;
        // point processing state
        bool point_power_pressed = false;
        bool point_log_pressed = false;
        bool point_invert_pressed = false;
        bool point_slice_pressed = false;
        bool point_bitplane_pressed = false;
        float point_gamma = 1.0f;
        // removed explicit C constants (computed internally)
        int point_slice_min = 0;
        int point_slice_max = 255;
        bool point_slice_preserve = true;
        bool slice_use_constant = true;
        int slice_constant_value = 255;
        int point_bit_preview_idx = 0; // plane to preview
        bool point_bitplane_apply_requested = false;
        SDL_Texture* point_bit_preview_tex = nullptr;
        int point_bit_preview_w = 0;
        int point_bit_preview_h = 0;

        std::shared_ptr<program::IAcquisitorService> acquisitor = program::ServiceManager::get<program::IAcquisitorService>();
        std::weak_ptr<toolbox::Asset> assetPtr;

        // functions
        void restrictWindowSize();
        void drawToolButton(enum ToolsButtons btn, const char* icon, bool hasPopup);
};
