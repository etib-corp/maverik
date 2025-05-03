/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Software
*/

#include "xr/Software.hpp"

maverik::xr::Software::Software(std::shared_ptr<PlatformData> platformData)
{
    _platform = std::make_shared<AndroidPlatform>(platformData);
    _graphicalContext = std::make_shared<maverik::xr::GraphicalContext>();
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
    strcpy(createInfo.applicationInfo.applicationName, "maverik");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    if (xrCreateInstance(&createInfo, &_XRinstance) != XR_SUCCESS) {
        return;
    }

    XrSystemGetInfo systemInfo{};
    systemInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    if (xrGetSystem(_XRinstance, &systemInfo, &_XRsystemID) != XR_SUCCESS) {
        return;
    }
    _graphicalContext->setXRInstance(_XRinstance);
    _graphicalContext->setXRSystemID(_XRsystemID);

    _graphicalContext->createInstance();
}
