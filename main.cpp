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

    if (!Graphics::WindowManager::init_context()) {
        return -1;
    }

    bool running = true;
    SDL_Event event;
    SDL_Texture *imgTexture = nullptr;
    int imgWidth = 0, imgHeight = 0;

    init_display_state(Graphics::WindowManager::window);

    // add all panels sto the window manager
    Graphics::WindowManager::register_panel<IMenuBarPanel>();
    Graphics::WindowManager::register_panel<IImagePreviewPanel>();

    // Main loop
    while (running) {
        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        SDL_Delay(16);

        Graphics::WindowManager::start_frame();
        Graphics::WindowManager::draw();
        Graphics::WindowManager::render_frame();
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(Graphics::WindowManager::renderer);
    SDL_DestroyWindow(Graphics::WindowManager::window);
    SDL_Quit();

    return 0;
}
