/*
** ETIB PROJECT, 2025
** maverik
** File description:
** vulkan
*/

#pragma once

    #include <vulkan/vulkan.hpp>

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

    #define GLM_FORCE_RADIANS
    #define GLM_FORCE_DEPTH_ZERO_TO_ONE
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #define GLM_ENABLE_EXPERIMENTAL
    #include <glm/gtx/hash.hpp>

    #include <GLFW/glfw3native.h>

    #define XIDER_VK_IMPLEMENTATION
    #define XIDER_XR_IMPLEMENTATION
