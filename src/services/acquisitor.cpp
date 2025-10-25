#include "services/acquisitor.hpp"

std::weak_ptr<toolbox::Asset> program::IAcquisitorService::getAssetByName(std::string name) {
    return toolbox::Acquisitor::assets[name];
}

std::weak_ptr<toolbox::Asset> program::IAcquisitorService::getLatestAsset() {
    return program::WindowState::currentAsset;
}

std::vector<std::weak_ptr<toolbox::Asset>> *program::IAcquisitorService::getAssets() {
    return &program::WindowState::assets;
}
