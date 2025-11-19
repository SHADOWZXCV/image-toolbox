#pragma once
#include "manager/panel.hpp"
#include "manager/state.hpp"
#include "shared/state.hpp"
#include "manager/window.hpp"
#include "fontawesome/IconsFontAwesome6.h"

struct IStatusBarPanel : public IPanel {
    unsigned int getID() const override { return id; }
    std::string getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; }

    void pre_draw() override {}
    bool show_condition() override { return true; }
    void draw() override;
    void handle_events() override {}

    static constexpr const char* name = "Status Bar";
private:
    // Account for left Tools panel (50px) and right Assets panel (250px)
    ImVec2 position{50.0f, (float)program::DISPLAY_HEIGHT - 24.0f};
    ImVec2 size{(float)program::DISPLAY_WIDTH - 50.0f - 250.0f, 24.0f};
    ImGuiWindowFlags imGuiFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse;
};
