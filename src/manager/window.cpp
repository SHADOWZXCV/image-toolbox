#pragma once
#include "manager/window.hpp"

using namespace Graphics;

SDL_Window* WindowManager::window;
SDL_Renderer* WindowManager::renderer;
std::vector<std::unique_ptr<IPanel>> WindowManager::panels;
std::vector<SDL_Texture*> WindowManager::owned_textures;
std::vector<Assets> WindowManager::assets;
std::queue<std::pair<std::string, unsigned int>> WindowManager::commands;
std::unordered_map<std::string, IPanel*> WindowManager::panel_map;

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

bool WindowManager::createVirtualWindow(std::string name, ImGuiWindowFlags flags) {
    return ImGui::Begin(name.c_str(),
    nullptr,
    flags);
}

bool WindowManager::renderPreviewImage(float zoom_percentage) {
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
        float max_w = width * zoom_percentage,
        max_h =  height * zoom_percentage;

        float scale = std::min(max_w / asset->image.cols, max_h / asset->image.rows);

        ImVec2 size = ImVec2(asset->image.cols * scale, asset->image.rows * scale);

        ImGui::SetCursorPos(ImVec2((width - size.x) / 2, (height - size.y) / 2));
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

void WindowManager::command_panel(std::pair<std::string, int> command) {
    auto it = WindowManager::panel_map.find(command.first);

    if (it == WindowManager::panel_map.end()) {
        return;
    }

    IPanel *panel = it->second;

    panel->panel_control_flags = command.second;
}

bool WindowManager::draw() {
    while (!WindowManager::commands.empty()) {
        std::pair<std::string, unsigned int> panel = WindowManager::commands.front();

        if (WindowManager::panel_map[panel.first]) {
            WindowManager::panel_map[panel.first]->panel_control_flags = panel.second;
        }

        WindowManager::commands.pop();
    }

    for(size_t i = 0; i < WindowManager::panels.size(); ++i) {
        auto& panel = WindowManager::panels[i];
        ImGui::SetNextWindowPos(panel->getPosition());
        ImGui::SetNextWindowSize(panel->getSize());
        WindowManager::createVirtualWindow(panel->getName(),
        panel->getImGuiFlags());

        panel->draw();

        WindowManager::endVirtualWindow();

        panel->panel_control_flags = 0;
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
