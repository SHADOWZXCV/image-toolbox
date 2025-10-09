#pragma once
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"

struct IImagePreviewPanel : public IPanel {
    unsigned int getID() const override { return id; }
    std::string getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void draw() override;
    void handle_events();

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
};
