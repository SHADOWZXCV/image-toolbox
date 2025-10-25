#include "renderer/image.hpp"

using namespace toolbox;

// bool ImageRenderer::load_image(const char *path) {
//     this->cv_image = cv::imread(path);
    
//     return this->cv_image.empty();
// }

ExplodedOpenCVMatrix *ImageRenderer::explode_image() {
    if (this->cv_image.empty())
        return nullptr;

    return &this->cv_image;
}

SDL_Texture* ImageRenderer::buildSDLTexture(SDL_Renderer *SDL_renderer, toolbox::Asset &asset) {
    toolbox::OpenCVProcessor::buildFinalImageFromAsset(asset);
    cv::Mat *image = &asset.displayed_image;

    int channels = image->channels();
    Uint32 pixel_format;

    cv::Mat temp;
    void* pixelData = (void*)image->data;
    int pitch = static_cast<int>(image->step);
    int bitsPerPixel = channels * 8;

    if (channels == 1) {
        cv::Mat grayImage(image->rows, image->cols, CV_8UC1, (void*)image->data, image->step);
        
        cv::cvtColor(grayImage, temp, cv::COLOR_GRAY2BGR);
        
        pixel_format = SDL_PIXELFORMAT_BGR24;
        pixelData = (void*)temp.data;
        pitch = static_cast<int>(temp.step);
        bitsPerPixel = temp.channels() * 8;
    }
    else if (channels == 3)
        pixel_format = SDL_PIXELFORMAT_BGR24;  // OpenCV stores 3-channel as BGR
    else if (channels == 4)
        pixel_format = SDL_PIXELFORMAT_BGRA32;
    else {
        std::cerr << "Unsupported image format!" << std::endl;

        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        pixelData,
        image->cols,
        image->rows,
        bitsPerPixel,
        pitch,
        pixel_format
    );

    if (!surface) {
        std::cerr << "SDL_CreateRGBSurfaceFrom failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL_renderer, surface);

    SDL_FreeSurface(surface);

    asset.SDL_texture = texture;

    return texture;
}
