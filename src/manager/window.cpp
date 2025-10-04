#pragma once
#include "manager/window.hpp"

using namespace Graphics;

SDL_Window* WindowManager::window;
SDL_Renderer* WindowManager::renderer;
std::vector<std::unique_ptr<IPanel>> WindowManager::panels;
std::vector<SDL_Texture*> WindowManager::owned_textures;
std::vector<Assets> WindowManager::assets;

std::string WindowManager::imagepath = "../assets/wow_toolbox.png";

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
    for(auto asset: WindowManager::assets) {
        SDL_DestroyTexture(asset.SDL_texture);
    }

    WindowManager::assets.clear();

    return true;
}

bool WindowManager::createVirtualWindow(const char* name, ImGuiWindowFlags flags) {
    return ImGui::Begin(name,
    nullptr,
    flags);
}

bool WindowManager::renderPreviewImage() {
    std::string path = WindowManager::getChosenImagePath();

    if (path.empty()) {
        ImGui::Text("No image is selected");

        return false;
    }

    Assets *asset = nullptr;

    for (auto &stored_asset: assets) {
        if (stored_asset.path == path) {
            asset = &stored_asset;
            break;
        }
    }

    if (!asset) {
        cleanup_old_textures();
        WindowManager::assets.clear();

        toolbox::ImageRenderer renderer = toolbox::ImageRenderer::buildSDLRenderer(WindowManager::renderer, path);

        Assets newAsset;
        newAsset.path = path;
        newAsset.image = renderer.cv_image;
        newAsset.SDL_texture = renderer.image_texture;

        WindowManager::assets.push_back(newAsset);

        asset = &WindowManager::assets.back();
    }

    if (asset->SDL_texture) {
        float width = ImGui::GetWindowSize().x;
        float height = ImGui::GetWindowSize().y;
        ImVec2 size = ImVec2(width - 2 * (width / 5), height - 2 * (height / 5));
        // ImVec2 size = ImVec2(DISPLAY_WIDTH - 2 * (DISPLAY_WIDTH / 5) - 20, DISPLAY_HEIGHT - 60);
        ImGui::Image((ImTextureID)asset->SDL_texture, size);
    } else {
        ImGui::Text("No image is selected");
    }

    return true;
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

void WindowManager::setChosenImagePath(std::string filepath) {
    WindowManager::imagepath = filepath;
}

std::string WindowManager::getChosenImagePath() {
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