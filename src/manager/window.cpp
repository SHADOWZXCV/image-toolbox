#pragma once
#include "manager/window.hpp"

using namespace toolbox;

bool WindowManager::start_frame() {
    try
    {
        // Start a new ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';

        return false;
    }    
}

bool WindowManager::render_frame(SDL_Renderer* renderer) {
    try
    {
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255); // Dark gray background
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}
bool WindowManager::createVirtualWindow(const char* name, ImGuiWindowFlags flags) {
    return ImGui::Begin(name,
    nullptr,
    flags);
}
bool WindowManager::showImage(SDL_Renderer *SDL_renderer, const char *path) {
    if (!path) {
        ImGui::Text("No image is selected");

        return false;
    }

    ImageRenderer renderer = ImageRenderer::buildSDLRenderer(SDL_renderer, path);

    if (!renderer.image_texture) {
        ImGui::Text("No image is selected");
    } else {
        ImGui::Image((ImTextureID)renderer.image_texture, ImVec2(ImGui::GetIO().DisplaySize.x - 2 * (ImGui::GetIO().DisplaySize.x / 5) - 20, ImGui::GetIO().DisplaySize.y - 60));
    }

    return true;
}
bool WindowManager::endVirtualWindow() {
    ImGui::End();
    
    return true;
}
