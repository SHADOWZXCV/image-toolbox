#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include "acquisitor/acquisitor.hpp"
#include "manager/window.hpp"

#include <iostream>

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow(
        "Toolbox",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    SDL_MaximizeWindow(window);

    if (!window) {
        std::cerr << "Error creating SDL_Window: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cerr << "Error creating SDL_Renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup ImGui SDL2/Renderer bindings
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool running = true;
    SDL_Event event;
    const char* filepath = nullptr;
    SDL_Texture *imgTexture = nullptr;
    int imgWidth = 0, imgHeight = 0;

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
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 30));
        toolbox::WindowManager::createVirtualWindow("File",
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f,0.2f,0.2f,0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f,0.3f,0.3f,0.3f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4,2)); 

        // Horizontal buttons
        if (ImGui::Button("Open")) {
            filepath = toolbox::Acquisitor::pick_file(false);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {}
        ImGui::SameLine();
        if (ImGui::Button("Settings")) {}

        if (imgTexture) {
            ImGui::Text("Selected: %s", filepath);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        toolbox::WindowManager::endVirtualWindow();

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 5, 30), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - 2 * (ImGui::GetIO().DisplaySize.x / 5), ImGui::GetIO().DisplaySize.y - 30), ImGuiCond_Always);
        toolbox::WindowManager::createVirtualWindow("Preview", 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBackground |
        ImGuiTableFlags_NoBordersInBody);


        toolbox::WindowManager::showImage(renderer, filepath);
        toolbox::WindowManager::endVirtualWindow();

        // Rendering
        toolbox::WindowManager::render_frame(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
