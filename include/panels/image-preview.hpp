#pragma once
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"

struct IImagePreviewPanel : public IPanel {
    char *getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void draw() override;

    private:
        // basic properties
        char* name = "Image Preview";
        ImVec2 position{(float) DISPLAY_WIDTH / 5, 30};
        ImVec2 size{(float) DISPLAY_WIDTH - 2 * (DISPLAY_WIDTH / 5), (float) DISPLAY_HEIGHT - 30};
        ImGuiWindowFlags imGuiFlags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiTableFlags_NoBordersInBody;
};
