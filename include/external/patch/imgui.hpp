#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace ImGui {
    bool ImGui::InputScalarNoText(const char* label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, ImGuiInputTextFlags flags);
}
