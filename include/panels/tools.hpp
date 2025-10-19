#pragma once
#include "fontawesome/IconsFontAwesome6.h"
#include "imgui_internal.h"
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "services/acquisitor.hpp"
#include "acquisitor/acquisitor.hpp"
#include "panels/image-preview.hpp"
#include "shared/state.hpp"

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

    char *getFilePath();

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

        // functions
        void restrictWindowSize();
};
