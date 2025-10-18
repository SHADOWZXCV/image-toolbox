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

SDL_Texture* ImageRenderer::buildSDLTexture(SDL_Renderer *SDL_renderer, toolbox::Asset *asset) {
    ExplodedOpenCVMatrix *image = &asset->image;

    int channels = image->channels();
    Uint32 pixel_format;

    if (channels == 1)
        pixel_format = SDL_PIXELFORMAT_RGB24;  // grayscale as 24-bit RGB
    else if (channels == 3)
        pixel_format = SDL_PIXELFORMAT_BGR24;  // OpenCV stores 3-channel as BGR
    else if (channels == 4)
        pixel_format = SDL_PIXELFORMAT_BGRA32;
    else {
        std::cerr << "Unsupported image format!" << std::endl;

        return nullptr;
    }

    // Note: For BGR/BGRA, OpenCV stores in memory exactly like SDL expects, no conversion needed
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        (void*)image->data,
        image->cols,
        image->rows,
        channels * 8,
        static_cast<int>(image->step),
        pixel_format
    );

    if (!surface) {
        std::cerr << "SDL_CreateRGBSurfaceFrom failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL_renderer, surface);

    SDL_FreeSurface(surface);

    asset->SDL_texture = texture;

    return texture;
}
