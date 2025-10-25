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
        cv::Mat original_image;
        cv::Mat displayed_image;
        // state
        cv::Mat transformation;
        bool dirty = false;

        bool setOriginalImage(cv::Mat image) {
            this->original_image = image;
            this->transformation = cv::Mat::eye(3, 3, CV_32F);

            return true;
        }
    };

    class Acquisitor {
        public:
            static std::unordered_map<std::string, std::shared_ptr<toolbox::Asset>> assets;
            static std::shared_ptr<toolbox::Asset> pick_image(bool multiple, std::vector<const char*> filters = {});
            static std::shared_ptr<toolbox::Asset> load_image(char *path);
    };
}
