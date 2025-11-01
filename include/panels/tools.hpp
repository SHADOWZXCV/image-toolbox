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

enum ToolsButtons {
    EQUALIZE_BUTTON = 0,
    GEO_TRANSFORMATION_BUTTON = 1
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
        float transformation_x = 0.0f;
        float transformation_y = 0.0f;
        float rotate_center[2] = { 0, 0 };
        float rotate_angle = 0;
        float prev_transformation_x = 0.0f;
        float prev_transformation_y = 0.0f;
        float prev_rotate_angle = 0;
        bool rotate_center_mouse_checked = false;
        bool skew_checked = false;
        bool slider_transform_x_changed = false;
        bool slider_transform_y_changed = false;
        bool rotate_center_changed = false;
        bool rotate_angle_changed = false;
        std::shared_ptr<program::IAcquisitorService> acquisitor = program::ServiceManager::get<program::IAcquisitorService>();
        std::weak_ptr<toolbox::Asset> assetPtr;

        // functions
        void restrictWindowSize();
};
