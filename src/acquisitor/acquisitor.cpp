#pragma once
#include "acquisitor/acquisitor.hpp"
#include "renderer/processor.hpp"
#include "renderer/transformations/color.hpp"

using namespace toolbox;

std::unordered_map<std::string, std::shared_ptr<toolbox::Asset>> Acquisitor::assets;

std::shared_ptr<toolbox::Asset> Acquisitor::pick_image(bool multiple, std::vector<const char*> filters) {
    const char* filter_array[] = {
        "*.jpg", "*.jpeg", "*.png", "*.bmp", "*.tiff", "*.tif", 
        "*.webp", "*.jp2", "*.pbm", "*.pgm", "*.ppm", "*.pxm", "*.pnm",
        "*.sr", "*.ras", "*.exr", "*.hdr", "*.gif", NULL
    };
    const char *path = const_cast<char *>(tinyfd_openFileDialog("Choose image", "", 2, filter_array, "image files", (int) multiple));
    std::shared_ptr<Asset> asset = Acquisitor::load_image(const_cast<char *>(path));

    return asset;
}

std::shared_ptr<toolbox::Asset> Acquisitor::load_image(char *path) {
    try
    {
        if (path == nullptr) {
            return nullptr;
        }

        cv::Mat image = cv::imread(path);

        if (image.empty()) {
            throw std::runtime_error("Image is not loaded from this path: " + std::string(path));
        }

        std::shared_ptr<Asset> asset = std::make_shared<Asset>();
        asset->setOriginalImage(image);
        asset->displayed_image = image;
        asset->path = path;

        Acquisitor::assets[path] = std::move(asset);

            // Initialize history, then apply initial operations (e.g., grayscale) to base_image
            Acquisitor::assets[path]->initHistory();
            toolbox::OpenCVProcessor::process<toolbox::GreyTransformation>(*Acquisitor::assets[path].get());

        return Acquisitor::assets[path];
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
}
