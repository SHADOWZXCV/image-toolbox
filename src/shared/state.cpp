#include "shared/state.hpp"
#include "acquisitor/acquisitor.hpp"

SDL_Event program::event;
int program::DISPLAY_WIDTH, program::DISPLAY_HEIGHT;
program::ControlsState program::WindowState::controlsState;
std::vector<std::unique_ptr<IPanel>> program::WindowState::panels;
std::vector<std::weak_ptr<toolbox::Asset>> program::WindowState::assets;
std::queue<std::pair<std::string, unsigned int>> program::WindowState::commands;
std::unordered_map<std::string, IPanel*> program::WindowState::panel_map;

bool program::WindowState::newAsset = false;
bool program::WindowState::textureUpdate = false;
std::weak_ptr<toolbox::Asset> program::WindowState::currentAsset;

void program::init_display_state(SDL_Window *window) {
    SDL_GetWindowSize(window, &DISPLAY_WIDTH, &DISPLAY_HEIGHT);
}

void program::setChosenAsset(std::shared_ptr<toolbox::Asset> asset) {
    program::WindowState::currentAsset = asset;
    program::WindowState::newAsset = true;
}

std::shared_ptr<toolbox::Asset> program::getChosenAsset() {
    return program::WindowState::currentAsset.lock();
}

void program::handleSDLEvents(bool *running) {
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            *running = false;
        if (event.type == SDL_KEYDOWN) {
            const Uint8* keyState = SDL_GetKeyboardState(NULL);
            bool ctrl = (SDL_GetModState() & KMOD_CTRL) != 0;
            bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;
            if (ctrl && event.key.keysym.sym == SDLK_z) {
                if (shift) {
                    // Redo
                    if (auto asset = program::getChosenAsset()) {
                        if (asset->redo()) {
                            program::WindowState::textureUpdate = true;
                        }
                    }
                } else {
                    // Undo
                    if (auto asset = program::getChosenAsset()) {
                        if (asset->undo()) {
                            program::WindowState::textureUpdate = true;
                        }
                    }
                }
            }

            // Toggle ROI selection (M), Crop mode (C), and cancel (Esc)
            if (event.key.keysym.sym == SDLK_m) {
                // Toggle marquee selection
                WindowState::controlsState.selectionFlags.roi_enabled = !WindowState::controlsState.selectionFlags.roi_enabled;
                if (!WindowState::controlsState.selectionFlags.roi_enabled) {
                    WindowState::controlsState.selectionFlags.crop_enabled = false;
                    WindowState::controlsState.selection = {};
                } else {
                    WindowState::controlsState.selection.is_dragging = false;
                    WindowState::controlsState.selection.has_roi = false;
                }
            }
            if (event.key.keysym.sym == SDLK_c) {
                // Toggle crop mode; implies ROI selection mode
                bool now = !WindowState::controlsState.selectionFlags.crop_enabled;
                WindowState::controlsState.selectionFlags.crop_enabled = now;
                WindowState::controlsState.selectionFlags.roi_enabled = now;
                WindowState::controlsState.selection.is_dragging = false;
                WindowState::controlsState.selection.has_roi = false;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                // Cancel selection/crop
                WindowState::controlsState.selectionFlags.roi_enabled = false;
                WindowState::controlsState.selectionFlags.crop_enabled = false;
                WindowState::controlsState.selection = {};
            }
        }
        
        if (event.type == SDL_DROPFILE) {
            char* droppedFilePath = event.drop.file;
            std::shared_ptr<toolbox::Asset> asset = toolbox::Acquisitor::load_image(droppedFilePath);

            if (asset) {
                setChosenAsset(std::move(asset));
            }
        }
    }
}
