#pragma once
#include <vector>
#include <SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <iostream>
#include "renderer/image.hpp"

namespace toolbox {
    class WindowManager {
        public:
            static bool start_frame();
            static bool render_frame(SDL_Renderer* renderer);
            static bool createVirtualWindow(const char* name, ImGuiWindowFlags flags);
            static bool showImage(SDL_Renderer *renderer, const char *path);
            static bool endVirtualWindow();
    }; 
}
