#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <utility>
#include <SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <iostream>
#include <vector>
#include "fontawesome/IconsFontAwesome6.h"
#include "renderer/image.hpp"
#include "manager/panel.hpp"
#include "shared/state.hpp"
#include "acquisitor/acquisitor.hpp"

namespace Graphics {

    extern void load_fonts();
    class WindowManager {
        public:
        static SDL_Window* window;
        static SDL_Renderer* renderer;

        template<typename TPanel>
        static IPanel* register_panel() {
            static_assert(std::is_base_of<IPanel, TPanel>::value, "TPanel must inherit IPanel");

            auto panel = std::make_unique<TPanel>();
            int id = static_cast<int>(program::WindowState::panels.size());
            TPanel* ptr = panel.get();

            program::WindowState::panel_map[ptr->name] = ptr;
            panel->id = id;
            program::WindowState::panels.push_back(std::move(panel));

            return ptr;
        }

        static void insert_texture(SDL_Texture *texture);
        static bool init_context();
        static bool start_frame();
        static bool render_frame();
        static bool renderPreviewImage(float zoom_percentage);
        static bool draw();
        static void command_panel(std::pair<std::string, int> command);
        
        private:
            program::WindowState window_state;
            
            static bool createVirtualWindow(std::string name, ImGuiWindowFlags flags);
            static bool endVirtualWindow();
            static bool cleanup_old_textures();
    }; 
}
