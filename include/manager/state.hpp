#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>
#include "acquisitor/acquisitor.hpp"

namespace program {
    struct IService {
        std::string name;
    };

    struct ServiceManager {
        static std::unordered_map<std::type_index, std::shared_ptr<IService>> services;

        template <typename TService>
        static void registerService() {
            static_assert(std::is_base_of<IService, TService>::value, "BAD SERVICE! TService must inherit IService");
            std::shared_ptr<TService> service = std::make_shared<TService>();

            ServiceManager::services[std::type_index(typeid(TService))] = std::move(service);
        }

        template <typename TService>
        static std::shared_ptr<TService> get() {
            auto it = ServiceManager::services.find(std::type_index(typeid(TService)));

            if (it == ServiceManager::services.end()) {
                return nullptr;
            }
            
            return std::static_pointer_cast<TService>(it->second);
        }
    };
}
