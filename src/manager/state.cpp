#include "manager/state.hpp"

using namespace program;

std::unordered_map<std::type_index, std::shared_ptr<IService>> ServiceManager::services;
