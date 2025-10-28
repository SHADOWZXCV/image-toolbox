#pragma once
#include "manager/window.hpp"

using namespace Graphics;

SDL_Window* WindowManager::window;
SDL_Renderer* WindowManager::renderer;

void Graphics::load_fonts() {
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // 1. Load your main font
    float baseFontSize = 16.0f;
    io.Fonts->AddFontFromFileTTF("include/assets/Roboto-Regular.ttf", baseFontSize);

    float iconFontSize = baseFontSize; // Icons are often bigger than letters, so scale down
    ImFontConfig config;
    config.MergeMode = true; // This is the important part!
    config.PixelSnapH = true;
    config.GlyphMinAdvanceX = iconFontSize; // Use if you want to make the icon monospaced

    // This defines the range of unicode characters to grab from the font.
    // ICON_MIN_FA and ICON_MAX_FA are defined in the header file.
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    io.Fonts->AddFontFromFileTTF("include/assets/fa-solid-900.ttf", iconFontSize, &config, icon_ranges);

}

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

bool WindowManager::cleanup_old_textures(toolbox::Asset &asset) {
    if (asset.SDL_texture) {
        SDL_DestroyTexture(asset.SDL_texture);
        asset.SDL_texture = nullptr;
        return true;
    }

    return false;
}

bool WindowManager::createVirtualWindow(std::string name, ImGuiWindowFlags flags) {
    return ImGui::Begin(name.c_str(),
    nullptr,
    flags);
}

std::weak_ptr<toolbox::Asset> WindowManager::renderPreviewImage(float zoom_percentage) {
    std::shared_ptr<toolbox::Asset> asset = program::getChosenAsset();
    std::weak_ptr<toolbox::Asset> observe_asset(asset);

    if (asset == nullptr) {
        ImGui::Text("No image is selected");

        return observe_asset;
    }

    if (program::WindowState::newAsset || program::WindowState::textureUpdate) {
        if (program::WindowState::textureUpdate) {
            cleanup_old_textures(*asset);
        } else if (program::WindowState::newAsset){
            // remove all previous assets for now
            for (auto it = program::WindowState::assets.begin(); it != program::WindowState::assets.end();) {
                std::shared_ptr<toolbox::Asset> locked_asset = it->lock();

                // removes any asset that's currently visible
                if (locked_asset) {
                    cleanup_old_textures(*locked_asset);                    
                }

                // removes any dangling weak pointer, not the actual asset
                it = program::WindowState::assets.erase(it);
            }
            for (std::weak_ptr<toolbox::Asset> old_asset : program::WindowState::assets) {

            }

            program::WindowState::assets.clear();
            program::WindowState::assets.push_back(asset);
            program::WindowState::newAsset = false;
        }

        toolbox::ImageRenderer::buildSDLTexture(WindowManager::renderer, *asset);

        ImGui::SetScrollX(0.0f);
        ImGui::SetScrollY(0.0f);
        zoom_percentage = 0.6f;

        if (program::WindowState::textureUpdate) {
            program::WindowState::textureUpdate = false;
        }
    }

    if (asset->SDL_texture) {
        float width = ImGui::GetWindowSize().x;
        float height = ImGui::GetWindowSize().y;
        float max_w = width * zoom_percentage,
        max_h =  height * zoom_percentage;

        float scale = std::min(max_w / asset->displayed_image.cols, max_h / asset->displayed_image.rows);
        ImVec2 size = ImVec2(asset->displayed_image.cols * scale, asset->displayed_image.rows * scale);
        ImVec2 position = ImVec2((width - size.x) / 2, (height - size.y) / 2);

        ImVec2 window = ImGui::GetWindowPos();

        asset->position = ImVec2(window.x + position.x, window.y + position.y);
        asset->size = size;

        ImGui::SetCursorPos(position);

        // ImVec2 size = ImVec2(DISPLAY_WIDTH - 2 * (DISPLAY_WIDTH / 5) - 20, DISPLAY_HEIGHT - 60);
        ImGui::Image((ImTextureID)asset.get()->SDL_texture, size);
    } else {
        ImGui::Text("No image is selected");
    }

    return observe_asset;
}

bool WindowManager::endVirtualWindow() {
    ImGui::End();
    
    return true;
}

void WindowManager::command_panel(std::pair<std::string, int> command) {
    auto it = program::WindowState::panel_map.find(command.first);

    if (it == program::WindowState::panel_map.end()) {
        return;
    }

    IPanel *panel = it->second;

    panel->panel_control_flags = command.second;
}

bool WindowManager::draw() {
    while (!program::WindowState::commands.empty()) {
        std::pair<std::string, unsigned int> panel = program::WindowState::commands.front();

        if (program::WindowState::panel_map[panel.first]) {
            program::WindowState::panel_map[panel.first]->panel_control_flags = panel.second;
        }

        program::WindowState::commands.pop();
    }

    for(size_t i = 0; i < program::WindowState::panels.size(); ++i) {
        auto& panel = program::WindowState::panels[i];

        bool show = panel->show_condition();

        if (!show)
            continue;

        ImGui::SetNextWindowPos(panel->getPosition(), ImGuiCond_Once);
        ImGui::SetNextWindowSize(panel->getSize(), ImGuiCond_Once);

        panel->pre_draw();
        
        WindowManager::createVirtualWindow(panel->getName(),
        panel->getImGuiFlags());
        
        panel->draw();

        WindowManager::endVirtualWindow();

        panel->panel_control_flags = 0;
    }

    return true;
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
    ImPlot::CreateContext();

    // Setup ImGui SDL2/Renderer bindings
    ImGui_ImplSDL2_InitForSDLRenderer(WindowManager::window, WindowManager::renderer);
    ImGui_ImplSDLRenderer2_Init(WindowManager::renderer);

    return true;
}
