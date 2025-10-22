#pragma once
#include "acquisitor/acquisitor.hpp"

using namespace toolbox;

std::unordered_map<std::string, std::unique_ptr<toolbox::Asset>> Acquisitor::assets;

toolbox::Asset *Acquisitor::pick_image(bool multiple, std::vector<const char*> filters) {
    const char* filter_array[] = {
        "*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff", "*.tif", 
        "*.webp", "*.jp2", "*.pbm", "*.pgm", "*.ppm", "*.pxm", "*.pnm",
        "*.sr", "*.ras", "*.exr", "*.hdr", "*.gif", NULL
    };
    const char *path = const_cast<char *>(tinyfd_openFileDialog("Choose image", "", 2, filter_array, "image files", (int) multiple));
    toolbox::Asset *asset = Acquisitor::load_image(const_cast<char *>(path));

    return asset;
}

toolbox::Asset *Acquisitor::load_image(char *path) {
    try
    {
        if (path == nullptr) {
            return nullptr;
        }

        cv::Mat image = cv::imread(path);

        if (image.empty()) {
            throw std::runtime_error("Image is not loaded from this path: " + std::string(path));
        }

        // Set it to grey
        toolbox::OpenCVProcessor::process<toolbox::GreyTransformation>(image);

        std::unique_ptr<Asset> asset = std::make_unique<Asset>();
        asset->image = image;
        asset->path = path;

        Acquisitor::assets[path] = std::move(asset);

        return Acquisitor::assets[path].get();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}
