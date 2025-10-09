#include "shared/state.hpp"

namespace program {
    SDL_Event event;
    int DISPLAY_WIDTH, DISPLAY_HEIGHT;

    void init_display_state(SDL_Window *window) {
        SDL_GetWindowSize(window, &DISPLAY_WIDTH, &DISPLAY_HEIGHT);
    }

    void handleSDLEvents(bool *running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                *running = false;
        }
    }
}
