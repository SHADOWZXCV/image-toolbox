#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "acquisitor/acquisitor.hpp"

namespace program {
    struct IService {
        std::string name;
    };

    struct ServiceManager {
        static std::unordered_map<std::string, std::unique_ptr<IService>> services;

        template <typename TService>
        static void registerService() {
            static_assert(std::is_base_of<IService, TService>::value, "BAD SERVICE! TService must inherit IService");
            std::unique_ptr<TService> service = std::make_unique<TService>();

            ServiceManager::services[service->name] = std::move(service);
        }

        static bool init();
        static IService *getByName(std::string name);
    };
}
