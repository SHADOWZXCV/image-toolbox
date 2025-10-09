#pragma once
#include <SDL.h>
#include <backends/imgui_impl_sdl2.h>

namespace program {
    extern SDL_Event event;
    extern int DISPLAY_WIDTH, DISPLAY_HEIGHT;
    void init_display_state(SDL_Window *window);
    void handleSDLEvents(bool *running);
}
