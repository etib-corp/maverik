/*
** ETIB PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#include "xr/GraphicalContext.hpp"

maverik::xr::GraphicalContext::GraphicalContext(const GraphicalContextPropertiesXR &properties)
    : _XRinstance(properties._XRinstance), _XRsystemID(properties._XRsystemID)
{
    createInstance();
}

maverik::xr::GraphicalContext::~GraphicalContext()
{
}

void maverik::xr::GraphicalContext::createInstance()
{
    XrGraphicsRequirementsVulkan2KHR graphicsRequirements{};
    PFN_xrGetVulkanGraphicsRequirements2KHR xrGetVulkanGraphicsRequirements2KHR = nullptr;
    PFN_xrCreateVulkanInstanceKHR xrCreateVulkanInstanceKHR = nullptr;

    graphicsRequirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR;
    if (xrGetInstanceProcAddr(_XRinstance, "xrGetVulkanGraphicsRequirements2KHR",
        reinterpret_cast<PFN_xrVoidFunction *>(&xrGetVulkanGraphicsRequirements2KHR)) != XR_SUCCESS) {
        std::cerr << "Failed to get xrGetVulkanGraphicsRequirements2KHR function" << std::endl;
        return;
    }
    if (xrGetVulkanGraphicsRequirements2KHR(_XRinstance, _XRsystemID, &graphicsRequirements) != XR_SUCCESS) {
        std::cerr << "Failed to get Vulkan graphics requirements" << std::endl;
        return;
    }
    if (xrGetInstanceProcAddr(_XRinstance, "xrCreateVulkanInstanceKHR",
        reinterpret_cast<PFN_xrVoidFunction *>(&xrCreateVulkanInstanceKHR)) != XR_SUCCESS) {
        std::cerr << "Failed to get xrCreateVulkanInstanceKHR function" << std::endl;
        return;
    }

    std::vector<const char *> layers{};
    std::vector<const char *> extensions{};

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName= "test",
        .applicationVersion = 0u,
        .pEngineName = "maverik",
        .engineVersion = 0u,
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo vkCreateInfo{};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkCreateInfo.pNext = nullptr;
    vkCreateInfo.flags = 0;
    vkCreateInfo.pApplicationInfo = &appInfo;
    vkCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    vkCreateInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();
    vkCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    vkCreateInfo.ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data();

    XrVulkanInstanceCreateInfoKHR vkInstanceCreateInfo{};
    vkInstanceCreateInfo.type = XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR;
    vkInstanceCreateInfo.systemId = _XRsystemID;
    vkInstanceCreateInfo.createFlags = 0;
    vkInstanceCreateInfo.pfnGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vkInstanceCreateInfo.vulkanCreateInfo = &vkCreateInfo;
    vkInstanceCreateInfo.vulkanAllocator = nullptr;

    VkResult result = VK_SUCCESS;
    if (xrCreateVulkanInstanceKHR(_XRinstance, &vkInstanceCreateInfo, &_instance, &result) != XR_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance" << std::endl;
        return;
    }
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance: " << result << std::endl;
        return;
    }

    RenderingContextPropertiesXR renderingProperties{};
    renderingProperties._XRinstance = _XRinstance;
    renderingProperties._XRsystemID = _XRsystemID;
    renderingProperties._vulkanInstance = _instance;

    _renderingContext = std::make_shared<maverik::xr::RenderingContext>(renderingProperties);
}

std::vector<std::string> maverik::xr::GraphicalContext::getInstanceExtensions()
{
    return {XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME};
}
