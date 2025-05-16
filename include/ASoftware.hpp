/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ASoftware
*/

#pragma once

#include "vulkan.hpp"
#include <iostream>
#include "AGraphicalContext.hpp"

#include "Version.hpp"

namespace maverik {
    class ASoftware {
        public:
            virtual ~ASoftware() = default;

        protected:
            std::string _appName;
            Version *_appVersion;
            std::string _engineName;
            Version *_engineVersion;

            std::shared_ptr<AGraphicalContext> _graphicalContext;
    };
}
