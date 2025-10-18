#pragma once
#include "imgui_internal.h"
#include "manager/panel.hpp"
#include "manager/window.hpp"
#include "services/acquisitor.hpp"
#include "acquisitor/acquisitor.hpp"
#include "panels/image-preview.hpp"
#include "shared/state.hpp"

struct IAssetsPanel : public IPanel {
    unsigned int getID() const override { return id; }
    std::string getName() const override { return name; }
    ImVec2 getPosition() const override { return resizedPosition; }
    ImVec2 getSize() const override { return resizedSize; }
    ImGuiWindowFlags getImGuiFlags() const override { return imGuiFlags; };
    void pre_draw() override;
    void draw() override;
    void handle_events() override;

    char *getFilePath();

    static constexpr const char* name = "Assets";
    private:
        // basic properties
        ImVec2 position{(float) program::DISPLAY_WIDTH - 250, 30};
        ImVec2 size{250, (float) program::DISPLAY_HEIGHT - 30};
        ImGuiWindowFlags imGuiFlags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar;
    
        // state
        ImVec2 resizedPosition = position;
        ImVec2 resizedSize = size;
        ImVec2 prevPosition = position;
        ImVec2 prevSize = size;

        // functions
        void restrictWindowSize();
};
