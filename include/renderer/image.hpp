#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include <SDL.h>
#include "acquisitor/acquisitor.hpp"
#include "renderer/processor.hpp"

typedef cv::Mat ExplodedOpenCVMatrix;
namespace toolbox {
    class ImageRenderer {
        public:
            cv::Mat cv_image;
            SDL_Texture *image_texture;
        
            static SDL_Texture *buildSDLTexture(SDL_Renderer *SDL_renderer, toolbox::Asset &asset);
            ExplodedOpenCVMatrix *explode_image();
            // bool load_image(const char* path);
    };
}

