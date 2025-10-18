#include "manager/state.hpp"

using namespace program;

std::unordered_map<std::string, std::unique_ptr<IService>> ServiceManager::services;

bool init();


IService *ServiceManager::getByName(std::string name) {
    auto it = ServiceManager::services.find(name);

    if (it == ServiceManager::services.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

bool ServiceManager::init() {
    // Not implemented yet. Not sure what to add here tbh!
    // for (auto &service : StateManager::services) {

    // }

    return false;
}
