#pragma once
#include <opencv2/opencv.hpp>
#include <SDL.h>

typedef cv::Mat ExplodedOpenCVMatrix;
namespace toolbox {
    class ImageRenderer {
        public:
            cv::Mat cv_image;
            SDL_Texture *image_texture;
        
            static ImageRenderer buildSDLRenderer(SDL_Renderer *SDL_renderer, const char *path);
            ExplodedOpenCVMatrix *explode_image();
            bool load_image(const char* path);
    };
}

