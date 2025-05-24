/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Software
*/

#include "xr/Software.hpp"

maverik::xr::Software::Software(const std::shared_ptr<PlatformData> &platformData)
{
    _platform = std::make_shared<AndroidPlatform>(platformData);
    _graphicalContext = nullptr;
}

maverik::xr::Software::~Software()
{
}

void maverik::xr::Software::createInstance()
{
    if (_XRinstance != XR_NULL_HANDLE)
        return;
    std::vector<const char *> extensions {
        XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
    };

    const std::vector<std::string> graphicsExtensions = _graphicalContext->getInstanceExtensions();
    std::transform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
        [](const std::string &ext) { return ext.c_str(); });

    XrInstanceCreateInfo createInfo{};
    createInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;
    createInfo.next = _platform->getInstanceCreateInfoAndroid();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.enabledExtensionNames = extensions.data();
    createInfo.enabledApiLayerCount = 0;
    createInfo.enabledApiLayerNames = nullptr;
    std::strcpy(createInfo.applicationInfo.applicationName, "maverik");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    if (xrCreateInstance(&createInfo, &_XRinstance) != XR_SUCCESS) {
        std::cerr << "Failed to create XR instance" << std::endl;
        return;
    }

    XrSystemGetInfo systemInfo{};
    systemInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    if (xrGetSystem(_XRinstance, &systemInfo, &_XRsystemID) != XR_SUCCESS) {
        std::cerr << "Failed to get XR system ID" << std::endl;
        return;
    }

    _graphicalContext = std::make_shared<maverik::xr::GraphicalContext>(_XRinstance, _XRsystemID);
    _graphicalContext->init();

    std::shared_ptr<VulkanContext> vulkanContext = _graphicalContext->getVulkanContext();
    if (vulkanContext == nullptr) {
        std::cerr << "Failed to get Vulkan context" << std::endl;
        return;
    }

    XrGraphicsBindingVulkan2KHR graphicsBinding{};

    graphicsBinding.type = XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR;
    graphicsBinding.next = nullptr;
    graphicsBinding.instance = _graphicalContext->getInstance();
    graphicsBinding.device = vulkanContext->logicalDevice;
    graphicsBinding.queueFamilyIndex = vulkanContext->graphicsQueueFamilyIndex;
    graphicsBinding.queueIndex = 0;

    XrSessionCreateInfo sessionInfo{};

    sessionInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionInfo.next = &graphicsBinding;
    sessionInfo.systemId = _XRsystemID;

    if (xrCreateSession(_XRinstance, &sessionInfo, &_XRsession) != XR_SUCCESS) {
        std::cerr << "Failed to create XR session" << std::endl;
        return;
    }
}
