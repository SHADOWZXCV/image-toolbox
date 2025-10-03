#pragma once
#include <imgui.h>

struct IPanel {
    virtual ~IPanel() = default;

    // basic properties
    virtual char *getName() const = 0;
    virtual ImVec2 getPosition() const = 0;
    virtual ImVec2 getSize() const = 0;
    virtual ImGuiWindowFlags getImGuiFlags() const = 0;

    virtual void draw() = 0;
};
