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

namespace maverik {
    class ASoftware {
        public:
            virtual ~ASoftware() = default;

            const std::string &getAppName() const {
                return _appName;
            }

            const std::string &getAppVersion() const {
                return _appVersion;
            }
        protected:
            std::string _appName;
            std::string _appVersion;

            std::shared_ptr<AGraphicalContext> _graphicalContext;
    };
}
