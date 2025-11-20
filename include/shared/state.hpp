#pragma once
#include <SDL.h>
#include <backends/imgui_impl_sdl2.h>
#include <vector>
#include "acquisitor/acquisitor.hpp"

namespace program {
    extern SDL_Event event;
    extern int DISPLAY_WIDTH, DISPLAY_HEIGHT;
    void init_display_state(SDL_Window *window);
    void handleSDLEvents(bool *running);

    struct BoolVec2 {
        BoolVec2(bool x_ = false, bool y_ = false) : x(x_), y(y_) {}
        bool x;
        bool y;
    };

    struct ControlsState {
        bool geoTransformEnabled;

        struct GeoTransformFlags {
            bool rotation_center_enabled;
            bool skew_enabled;
            bool flip_command_enabled;
            bool scale_enabled;
            bool translate_enabled; // mouse translation
        } geoTransformFlags;
        struct GeoTransformState {
            ImVec2 rotation_center;
            BoolVec2 flip;
            BoolVec2 scale;
        } geoTransform;

        // ROI / Crop selection controls
        struct SelectionFlags {
            bool roi_enabled = false;   // marquee selection mode
            bool crop_enabled = false;  // crop mode (uses marquee)
        } selectionFlags;
        struct SelectionState {
            bool is_dragging = false;   // currently dragging selection
            bool has_roi = false;       // a valid ROI exists
            ImVec2 start_img{0,0};      // ROI start in image coordinates
            ImVec2 end_img{0,0};        // ROI end in image coordinates
        } selection;
    };

    struct WindowState {
        static ControlsState controlsState;
        static std::weak_ptr<toolbox::Asset> currentAsset;
        static bool newAsset;
        static bool textureUpdate;
        static std::vector<std::weak_ptr<toolbox::Asset>> assets;
        static std::vector<std::unique_ptr<IPanel>> panels;
        static std::unordered_map<std::string, IPanel*> panel_map;
        static std::queue<std::pair<std::string, unsigned int>> commands;
    };

    std::shared_ptr<toolbox::Asset> getChosenAsset();
    void setChosenAsset(std::shared_ptr<toolbox::Asset> asset);
    /**
     * A panel wants to request some state.
     * There are different implementations for this:
     * - Option 1: Have a decentralized state manager for each panel, and a delivery manager.
     *             A delivery manager is responsible for communication between panels' state managers.
     * For example: 
     *  Panel X: I want to have the current image drawn on the panel A
     *  Panel A: Here you go!
     * - Option 2: Have a centralized manager, that has all the state, and it is the responsibility of
     *             each panel to register stuff onto the state manager.
     * Can be in a different way too!
     * Example: 
     */
}
