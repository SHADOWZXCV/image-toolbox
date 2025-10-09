#pragma once
#include <imgui.h>
#include <string>

enum PANEL_CONTROL_FLAGS {
    RESET_PANEL_FLAG = 1,
    RESET_ZOOM_FLAG = 2
};

struct IPanel {
    virtual ~IPanel() = default;

    // basic properties
    virtual unsigned int getID() const = 0;
    virtual std::string getName() const = 0;
    virtual ImVec2 getPosition() const = 0;
    virtual ImVec2 getSize() const = 0;
    virtual ImGuiWindowFlags getImGuiFlags() const = 0;

    virtual void draw() = 0;

    int panel_control_flags = 0;
    int id = 0;
};
