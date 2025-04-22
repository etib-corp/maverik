/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AGraphicalContext
*/

#pragma once

#include "ARenderingContext.hpp"
#include "ASwapchainContext.hpp"

#include <memory>

namespace maverik {
    class AGraphicalContext {
        public:
        virtual ~AGraphicalContext() = default;

        virtual void init() = 0;
        virtual void run() = 0;
        virtual void getInstanceExtension() = 0;

        protected:
        virtual void createInstance() = 0;

        std::shared_ptr<ARenderingContext> _renderingContext;
        std::shared_ptr<ASwapchainContext> _swapchainContext;
        VkInstance _instance;
    };
}    // namespace maverik