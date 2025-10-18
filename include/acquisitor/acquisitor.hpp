#pragma once
#include <vector>
#include <SDL.h>
#include <opencv2/opencv.hpp>
extern "C" {
    #include <tinyfiledialogs.h>
}

#include "manager/panel.hpp"
#include "manager/state.hpp"

namespace toolbox {
    struct Asset {
        std::string path;
        SDL_Texture *SDL_texture;
        cv::Mat image;
    };

    class Acquisitor {
        public:
            static std::unordered_map<std::string, std::unique_ptr<toolbox::Asset>> assets;
            static toolbox::Asset *pick_image(bool multiple, std::vector<const char*> filters = {});
            static toolbox::Asset *load_image(char *path);
    };
}
