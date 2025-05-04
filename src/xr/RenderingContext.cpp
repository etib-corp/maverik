/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#include "xr/RenderingContext.hpp"

maverik::xr::RenderingContext::RenderingContext(XrInstance instance, XrSystemId systemID)
    : _XRinstance(instance), _XRsystemID(systemID)
{
}

maverik::xr::RenderingContext::~RenderingContext()
{
}

void maverik::xr::RenderingContext::init()
{
    PFN_xrGetVulkanGraphicsDevice2KHR xrGetVulkanGraphicsDevice2KHR = nullptr;
    PFN_xrCreateVulkanDeviceKHR xrCreateVulkanDeviceKHR = nullptr;
    XrVulkanGraphicsDeviceGetInfoKHR graphicsDeviceGetInfo{};

    if (_XRinstance == XR_NULL_HANDLE) {
        return;
    }
    if (xrGetInstanceProcAddr(_XRinstance, "xrGetVulkanGraphicsDevice2KHR",
        reinterpret_cast<PFN_xrVoidFunction *>(&xrGetVulkanGraphicsDevice2KHR)) != XR_SUCCESS) {
            std::cerr << "Failed to get xrGetVulkanGraphicsDevice2KHR function" << std::endl;
            return;
    }
    graphicsDeviceGetInfo.type = XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR;
    graphicsDeviceGetInfo.systemId = _XRsystemID;
    graphicsDeviceGetInfo.vulkanInstance = _vulkanInstance;

    if (xrGetVulkanGraphicsDevice2KHR(_XRinstance, &graphicsDeviceGetInfo, &_vulkanPhysicalDevice) != XR_SUCCESS) {
        return;
    }

    if (xrGetInstanceProcAddr(_XRinstance, "xrCreateVulkanDeviceKHR",
        reinterpret_cast<PFN_xrVoidFunction *>(&xrCreateVulkanDeviceKHR)) != XR_SUCCESS) {
            std::cerr << "Failed to get xrCreateVulkanDeviceKHR function" << std::endl;
            return;
    }
    float queuePriority = 0.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    uint32_t queueFamilyIndex = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_vulkanPhysicalDevice, &queueFamilyIndex, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyIndex);
    vkGetPhysicalDeviceQueueFamilyProperties(_vulkanPhysicalDevice, &queueFamilyIndex, &queueFamilyProperties[0]);
    for (uint32_t i = 0; i < queueFamilyIndex; ++i) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueCreateInfo.queueFamilyIndex = i;
            break;
        }
    }
    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &features;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;

    XrVulkanDeviceCreateInfoKHR vulkanDeviceCreateInfo{};
    vulkanDeviceCreateInfo.type = XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR;
    vulkanDeviceCreateInfo.next = &deviceCreateInfo;
    vulkanDeviceCreateInfo.systemId = _XRsystemID;
    vulkanDeviceCreateInfo.createFlags = 0;
    vulkanDeviceCreateInfo.pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanDeviceCreateInfo.vulkanPhysicalDevice = _vulkanPhysicalDevice;
    vulkanDeviceCreateInfo.vulkanCreateInfo = &deviceCreateInfo;
    vulkanDeviceCreateInfo.vulkanAllocator = nullptr;

    VkResult result = VK_SUCCESS;
    if (xrCreateVulkanDeviceKHR(_XRinstance, &vulkanDeviceCreateInfo, &_vulkanDevice, &result) != XR_SUCCESS) {
        std::cerr << "Failed to create Vulkan device" << std::endl;
        return;
    }
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan device: " << result << std::endl;
        return;
    }
    if (vkGetDeviceQueue(_vulkanDevice, queueCreateInfo.queueFamilyIndex, 0, &_vulkanQueue) != VK_SUCCESS) {
        std::cerr << "Failed to get Vulkan queue" << std::endl;
        return;
    }

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueCreateInfo.queueFamilyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(_vulkanDevice, &commandPoolCreateInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan command pool" << std::endl;
        return;
    }
}
