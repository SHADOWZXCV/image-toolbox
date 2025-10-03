#include "shared/state.hpp"

int DISPLAY_WIDTH, DISPLAY_HEIGHT;

void init_display_state(SDL_Window *window) {
    SDL_GetWindowSize(window, &DISPLAY_WIDTH, &DISPLAY_HEIGHT);
}
