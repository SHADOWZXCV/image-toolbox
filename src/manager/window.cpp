#pragma once
#include "manager/window.hpp"

using namespace Graphics;

SDL_Window* WindowManager::window;
SDL_Renderer* WindowManager::renderer;
std::vector<std::unique_ptr<IPanel>> WindowManager::panels;
std::vector<SDL_Texture*> WindowManager::owned_textures;
std::vector<Assets> WindowManager::assets;

char *WindowManager::imagepath = "../assets/wow_toolbox.png";

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

bool WindowManager::render_frame() {
    try
    {
        ImGui::Render();
        SDL_SetRenderDrawColor(WindowManager::renderer, 25, 25, 25, 255); // Dark gray background
        SDL_RenderClear(WindowManager::renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), WindowManager::renderer);
        SDL_RenderPresent(WindowManager::renderer);

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool WindowManager::cleanup_old_textures() {
    for(auto texture: WindowManager::owned_textures) {
        SDL_DestroyTexture(texture);
    }

    WindowManager::owned_textures.clear();

    return true;
}

bool WindowManager::createVirtualWindow(const char* name, ImGuiWindowFlags flags) {
    return ImGui::Begin(name,
    nullptr,
    flags);
}

bool WindowManager::renderPreviewImage() {
    char *path = WindowManager::getChosenImagePath();

    if (!path) {
        ImGui::Text("No image is selected");

        return false;
    }

    SDL_Texture *texture = nullptr;
    bool assetExists = false;

    for (auto &asset: assets) {
        if (asset.path == path) {
            texture = asset.SDL_texture;
            assetExists = true;
            break;
        }
    }

    if (!assetExists) {
        cleanup_old_textures();

        toolbox::ImageRenderer renderer = toolbox::ImageRenderer::buildSDLRenderer(WindowManager::renderer, path);
        texture = renderer.image_texture;

        insert_texture(texture);
    }

    if (texture) {
        ImGui::Image((ImTextureID)texture, ImVec2(ImGui::GetIO().DisplaySize.x - 2 * (ImGui::GetIO().DisplaySize.x / 5) - 20, ImGui::GetIO().DisplaySize.y - 60));
    } else {
        ImGui::Text("No image is selected");
    }

    return true;
}

void WindowManager::insert_texture(SDL_Texture *texture) {
    if (texture) WindowManager::owned_textures.push_back(texture);
}


bool WindowManager::endVirtualWindow() {
    ImGui::End();
    
    return true;
}

bool WindowManager::draw() {
    for(auto &panel: WindowManager::panels) {
        ImGui::SetNextWindowPos(panel->getPosition());
        ImGui::SetNextWindowSize(panel->getSize());
        WindowManager::createVirtualWindow(panel->getName(),
        panel->getImGuiFlags());

        panel->draw();

        WindowManager::endVirtualWindow();
    }

    return true;
}

void WindowManager::setChosenImagePath(char *filepath) {
    WindowManager::imagepath = filepath;
}

char *WindowManager::getChosenImagePath() {
    return WindowManager::imagepath;
}

bool WindowManager::init_context() {
    // Create SDL window
    WindowManager::window = SDL_CreateWindow(
        "Toolbox",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    SDL_MaximizeWindow(WindowManager::window);

    if (!WindowManager::window) {
        std::cerr << "Error creating SDL_Window: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create SDL renderer
    WindowManager::renderer = SDL_CreateRenderer(WindowManager::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!WindowManager::renderer) {
        std::cerr << "Error creating SDL_Renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup ImGui SDL2/Renderer bindings
    ImGui_ImplSDL2_InitForSDLRenderer(WindowManager::window, WindowManager::renderer);
    ImGui_ImplSDLRenderer2_Init(WindowManager::renderer);

    return true;
}