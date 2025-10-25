#pragma once
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"

namespace program {
    struct IAcquisitorService : public IService {
        std::string name = "acquisitor_service";

        std::weak_ptr<toolbox::Asset> getAssetByName(std::string);
        std::weak_ptr<toolbox::Asset> getLatestAsset();
        std::vector<std::weak_ptr<toolbox::Asset>> *getAssets();
    };
}
