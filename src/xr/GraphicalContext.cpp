/*
** ETIB PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#include "xr/GraphicalContext.hpp"

maverik::xr::GraphicalContext::GraphicalContext()
{
}

maverik::xr::GraphicalContext::~GraphicalContext()
{
}

std::vector<std::string> maverik::xr::GraphicalContext::getInstanceExtensions() override
{
    return {XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME}
}
