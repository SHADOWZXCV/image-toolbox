#pragma once
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "manager/state.hpp"
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"
#include "renderer/processor.hpp"
#include "renderer/transformations/geometric.hpp"

#define TOP_EDGE cv::Point2d(4, 5)
#define RIGHT_EDGE cv::Point2d(5, 6)
#define BOTTOM_EDGE cv::Point2d(6, 7)
#define LEFT_EDGE cv::Point2d(7, 4)

struct IImagePreviewPanel : public IPanel {
    unsigned int getID() const override { return id; }
    std::string getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void pre_draw() override;
    void draw() override;
    void handle_events() override;
    bool show_condition() override;
    ImVec2* transformImageCorners(ImVec2 position, cv::MatSize size,  cv::Mat transformation);

    static constexpr const char* name = "Image Preview";
    private:
        // basic properties
        ImVec2 position{(float) program::DISPLAY_WIDTH / 5, 30};
        ImVec2 size{(float) program::DISPLAY_WIDTH - 2 * (program::DISPLAY_WIDTH / 5), (float) program::DISPLAY_HEIGHT - 30};
        ImGuiWindowFlags imGuiFlags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiTableFlags_NoBordersInBody;

        // state
        std::vector<std::weak_ptr<toolbox::Asset>> assets;
        std::weak_ptr<toolbox::Asset> assetWeak;
        bool free_rotate_switch = false;
        bool edge_held = false;
        float prev_rotation_angle = 0;
        ImVec2 edge_held_points[2];
        ImVec2 mouse_skew_delta;
        ImVec2 prev_mouse_skew_delta = ImVec2();
        ImVec2 handles_abs_positions[8];
};
