#include "services/acquisitor.hpp"

toolbox::Asset *program::IAcquisitorService::getAssetByName(std::string name) {
    return toolbox::Acquisitor::assets[name].get();
}

toolbox::Asset *program::IAcquisitorService::getLatestAsset() {
    return program::WindowState::currentAsset;
}

std::vector<toolbox::Asset *> *program::IAcquisitorService::getAssets() {
    return &program::WindowState::assets;
}
