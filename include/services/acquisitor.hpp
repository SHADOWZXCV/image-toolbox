#pragma once
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"

namespace program {
    struct IAcquisitorService : public IService {
        std::string name = "acquisitor_service";

        toolbox::Asset *getAssetByName(std::string);
        toolbox::Asset *getLatestAsset();
        std::vector<toolbox::Asset *> *getAssets();
    };
}
