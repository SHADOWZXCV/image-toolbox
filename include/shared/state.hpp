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

    struct WindowState {
        static toolbox::Asset *currentAsset;
        static bool newAsset;
        static bool textureUpdate;
        static std::vector<SDL_Texture*> owned_textures;
        static std::vector<toolbox::Asset *> assets;
        static std::vector<std::unique_ptr<IPanel>> panels;
        static std::unordered_map<std::string, IPanel*> panel_map;
        static std::queue<std::pair<std::string, unsigned int>> commands;
    };

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
