#include <iostream>
#include "manager/window.hpp"
#include "panels/menubar.hpp"
#include "panels/image-preview.hpp"

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!toolbox::WindowManager::init_context()) {
        return -1;
    }

    bool running = true;
    SDL_Event event;
    SDL_Texture *imgTexture = nullptr;
    int imgWidth = 0, imgHeight = 0;

    init_display_state(toolbox::WindowManager::window);

    // add all panels sto the window manager
    toolbox::WindowManager::register_panel<IMenuBarPanel>();
    toolbox::WindowManager::register_panel<IImagePreviewPanel>();

    // Main loop
    while (running) {
        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        toolbox::WindowManager::start_frame();
        toolbox::WindowManager::draw();
        toolbox::WindowManager::render_frame();
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(toolbox::WindowManager::renderer);
    SDL_DestroyWindow(toolbox::WindowManager::window);
    SDL_Quit();

    return 0;
}
