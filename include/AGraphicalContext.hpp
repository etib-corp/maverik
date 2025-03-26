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

template <typename T>
class AGraphicalContext {
    public:
        virtual ~AGraphicalContext() = default;
    protected:
        std::shared_ptr<ARenderingContext> _renderingContext;
        std::shared_ptr<ASwapchainContext> _swapchainContext;
        T _instance;
};