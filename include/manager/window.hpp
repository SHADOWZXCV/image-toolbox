#pragma once
#include <vector>
#include <SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <iostream>
#include <vector>
#include "renderer/image.hpp"
#include "manager/panel.hpp"


namespace toolbox {
    class WindowManager {
        public:
        static SDL_Window* window;
        static SDL_Renderer* renderer;
        template<typename TPanel>
        static bool register_panel() {
            static_assert(std::is_base_of<IPanel, TPanel>::value, "TPanel must inherit IPanel");
            std::cout << DISPLAY_WIDTH << std::endl;
            WindowManager::panels.push_back(std::make_unique<TPanel>());

            return true;
        }
        static bool init_context();
        static bool start_frame();
        static bool render_frame();
        static bool renderPreviewImage();
        static bool draw();
        static void setChosenImagePath(char *filepath);
        static char *getChosenImagePath();

        private:
            static char *imagepath;
            static std::vector<std::unique_ptr<IPanel>> panels;

            static bool createVirtualWindow(const char* name, ImGuiWindowFlags flags);
            static bool endVirtualWindow();
    }; 
}
