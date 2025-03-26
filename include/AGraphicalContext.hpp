/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AGraphicalContext
*/

#pragma once

#include "ASwapchainContext.hpp"
#include "ARenderingContext.hpp"

#include <memory>


namespace maverik {
    template <typename T>
    class AGraphicalContext {
        public:
            virtual ~AGraphicalContext() = default;

            virtual void init() = 0;
            virtual void run() = 0;
        protected:
            virtual void createInstance() = 0;

            std::shared_ptr<ARenderingContext> _renderingContext;
            std::shared_ptr<ASwapchainContext> _swapchainContext;
            T _instance;
    };
}