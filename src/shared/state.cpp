#include "shared/state.hpp"
#include "acquisitor/acquisitor.hpp"

SDL_Event program::event;
int program::DISPLAY_WIDTH, program::DISPLAY_HEIGHT;
std::vector<std::unique_ptr<IPanel>> program::WindowState::panels;
std::vector<SDL_Texture*> program::WindowState::owned_textures;
std::vector<toolbox::Asset *> program::WindowState::assets;
std::queue<std::pair<std::string, unsigned int>> program::WindowState::commands;
std::unordered_map<std::string, IPanel*> program::WindowState::panel_map;

bool program::WindowState::newAsset = false;
toolbox::Asset *program::WindowState::currentAsset = nullptr;

void program::init_display_state(SDL_Window *window) {
    SDL_GetWindowSize(window, &DISPLAY_WIDTH, &DISPLAY_HEIGHT);
}

void setChosenAsset(toolbox::Asset *asset) {
    program::WindowState::currentAsset = asset;
    program::WindowState::newAsset = true;
}

toolbox::Asset *getChosenAsset() {
    return program::WindowState::currentAsset;
}

void program::handleSDLEvents(bool *running) {
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            *running = false;
        
        if (event.type == SDL_DROPFILE) {
            char* droppedFilePath = event.drop.file;
            toolbox::Asset *asset = toolbox::Acquisitor::load_image(droppedFilePath);

            if (asset) {
                setChosenAsset(asset);
            }
        }
    }
}
