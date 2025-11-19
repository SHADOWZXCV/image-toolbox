#include "shared/theme.hpp"

#include <imgui.h>

namespace theme {

static void ApplyCommon(ImVec4 bg,
                        ImVec4 childBg,
                        ImVec4 popupBg,
                        ImVec4 text,
                        ImVec4 textDisabled,
                        ImVec4 border,
                        ImVec4 borderShadow,
                        ImVec4 accent,
                        ImVec4 accentDark,
                        ImVec4 accentHover,
                        ImVec4 deepBlue)
{
    ImGuiStyle &style = ImGui::GetStyle();

    // Frames
    style.Colors[ImGuiCol_FrameBg]          = childBg;
    style.Colors[ImGuiCol_FrameBgHovered]   = accentHover;
    style.Colors[ImGuiCol_FrameBgActive]    = accentDark;

    // Buttons
    style.Colors[ImGuiCol_Button]           = bg;
    style.Colors[ImGuiCol_ButtonHovered]    = accent;
    style.Colors[ImGuiCol_ButtonActive]     = accentDark;

    // Sliders
    style.Colors[ImGuiCol_SliderGrab]       = accent;
    style.Colors[ImGuiCol_SliderGrabActive] = accentDark;

    // Headers
    style.Colors[ImGuiCol_Header]           = deepBlue;
    style.Colors[ImGuiCol_HeaderHovered]    = accentHover;
    style.Colors[ImGuiCol_HeaderActive]     = accentDark;

    // Tabs
    style.Colors[ImGuiCol_Tab]              = bg;
    style.Colors[ImGuiCol_TabHovered]       = deepBlue;
    style.Colors[ImGuiCol_TabActive]        = accent;
    style.Colors[ImGuiCol_TabUnfocused]     = bg;

    // Windows / backgrounds
    style.Colors[ImGuiCol_WindowBg]         = bg;
    style.Colors[ImGuiCol_ChildBg]          = childBg;
    style.Colors[ImGuiCol_PopupBg]          = popupBg;
    style.Colors[ImGuiCol_MenuBarBg]        = childBg;
    style.Colors[ImGuiCol_DockingEmptyBg]   = bg;
    style.Colors[ImGuiCol_DockingPreview]   = accentHover;

    // Separators
    style.Colors[ImGuiCol_Separator]        = accentDark;
    style.Colors[ImGuiCol_SeparatorHovered] = accentHover;
    style.Colors[ImGuiCol_SeparatorActive]  = accent;

    // Misc
    style.Colors[ImGuiCol_CheckMark]        = accent;

    // Title bar
    style.Colors[ImGuiCol_TitleBg]          = deepBlue;
    style.Colors[ImGuiCol_TitleBgActive]    = accentDark;
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0,0,0,0.5f);

    // Text & Borders
    style.Colors[ImGuiCol_Text]             = text;
    style.Colors[ImGuiCol_TextDisabled]     = textDisabled;
    style.Colors[ImGuiCol_Border]           = border;
    style.Colors[ImGuiCol_BorderShadow]     = borderShadow;
}

void ApplyLightTheme() {
    ImVec4 bg           = ImVec4(0.92f, 0.93f, 0.96f, 1.f);
    ImVec4 childBg      = ImVec4(0.95f, 0.96f, 0.98f, 1.f);
    ImVec4 popupBg      = ImVec4(0.97f, 0.97f, 0.99f, 1.f);

    ImVec4 text         = ImVec4(0.12f, 0.12f, 0.15f, 1.f);
    ImVec4 textDisabled = ImVec4(0.55f, 0.55f, 0.60f, 1.f);

    ImVec4 border       = ImVec4(0.70f, 0.72f, 0.78f, 1.f);
    ImVec4 borderShadow = ImVec4(0,0,0,0);

    ImVec4 accent       = ImVec4(0.55f, 0.60f, 0.70f, 0.7f);
    ImVec4 accentDark   = ImVec4(0.48f, 0.53f, 0.63f, 1.f);
    ImVec4 accentHover  = ImVec4(0.62f, 0.67f, 0.75f, 0.4f);
    ImVec4 deepBlue     = ImVec4(0.30f, 0.35f, 0.55f, 1.f);

    ApplyCommon(bg, childBg, popupBg, text, textDisabled, border, borderShadow, accent, accentDark, accentHover, deepBlue);
}

void ApplyDarkTheme() {
    ImVec4 bg           = ImVec4(0.05f, 0.05f, 0.08f, 1.f);
    ImVec4 childBg      = ImVec4(0.07f, 0.06f, 0.10f, 1.f);
    ImVec4 popupBg      = ImVec4(0.08f, 0.07f, 0.12f, 1.f);

    ImVec4 text         = ImVec4(0.90f, 0.90f, 0.95f, 1.f);
    ImVec4 textDisabled = ImVec4(0.40f, 0.40f, 0.45f, 1.f);

    ImVec4 border       = ImVec4(0.18f, 0.18f, 0.25f, 1.f);
    ImVec4 borderShadow = ImVec4(0,0,0,0);

    ImVec4 accent       = ImVec4(0.35f, 0.40f, 0.50f, 0.7f);
    ImVec4 accentDark   = ImVec4(0.28f, 0.33f, 0.43f, 1.f);
    ImVec4 accentHover  = ImVec4(0.40f, 0.45f, 0.55f, 0.4f);
    ImVec4 deepBlue     = ImVec4(0.15f, 0.15f, 0.35f, 1.f);

    ApplyCommon(bg, childBg, popupBg, text, textDisabled, border, borderShadow, accent, accentDark, accentHover, deepBlue);
}

} // namespace theme
