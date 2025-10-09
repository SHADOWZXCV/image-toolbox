#pragma once
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"
#include "panels/image-preview.hpp"
struct IMenuBarPanel : public IPanel {
    unsigned int getID() const override { return id; }
    std::string getName() const override { return name; }
    ImVec2 getPosition() const override { return position; }
    ImVec2 getSize() const override { return size; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void draw() override;

    char *getFilePath();

    static constexpr const char* name = "MenuBar";
    private:
        // basic properties
        ImVec2 position{0, 0};
        ImVec2 size{(float) program::DISPLAY_WIDTH, 30};
        ImGuiWindowFlags imGuiFlags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar;

        // state
        char* filepath = nullptr;
};