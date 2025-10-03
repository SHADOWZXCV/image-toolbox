#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include "acquisitor/acquisitor.hpp"
#include "manager/window.hpp"
#include "panels/menubar.hpp"
#include "panels/image-preview.hpp"
#include "shared/state.hpp"

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    toolbox::WindowManager::init_context();

    bool running = true;
    SDL_Event event;
    SDL_Texture *imgTexture = nullptr;
    int imgWidth = 0, imgHeight = 0;

    init_display_state(toolbox::WindowManager::window);

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

        // Your GUI

        toolbox::WindowManager::draw();

        // ImGui::SetNextWindowPos(ImVec2(DISPLAY_WIDTH / 5, 30), ImGuiCond_Always);
        // ImGui::SetNextWindowSize(ImVec2(DISPLAY_WIDTH - 2 * (DISPLAY_WIDTH / 5), DISPLAY_HEIGHT - 30), ImGuiCond_Always);
        // toolbox::WindowManager::createVirtualWindow("Preview", 
        // ImGuiWindowFlags_NoTitleBar |
        // ImGuiWindowFlags_NoMove |
        // ImGuiWindowFlags_NoResize |
        // ImGuiWindowFlags_NoCollapse |
        // ImGuiWindowFlags_NoBackground |
        // ImGuiTableFlags_NoBordersInBody);


        // toolbox::WindowManager::showImage(renderer, filepath);
        // toolbox::WindowManager::endVirtualWindow();

        // Rendering
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
