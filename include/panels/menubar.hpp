#pragma once
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"

struct IMenuBarPanel : public IPanel {
    char *getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void draw() override;

    char *getFilePath();

    private:
        // basic properties
        char* name = "MenuBar";
        ImVec2 position{0, 0};
        ImVec2 size{(float) DISPLAY_WIDTH, 30};
        ImGuiWindowFlags imGuiFlags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar;

        // state
        char* filepath = nullptr;
};