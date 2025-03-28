/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Software
*/

#include "xr/Software.hpp"

maverik::xr::Software::Software(struct android_app *app)
{
    _platform = std::make_shared<AndroidPlatform>(app);
    _graphicalContext = std::make_shared<GraphicalContext>();
}

maverik::xr::Software::~Software()
{
}

maverik::xr::Software::createInstance()
{
    if (_XRinstance != XR_NULL_HANDLE)
        return;
    std::vector<const char *> extensions {
        XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME
    };

    const std::string graphicsExtensions = _graphicalContext->getInstanceExtensions();
    std::tranform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
        [](const std::string &ext) { return ext.c_str(); });

    XrInstanceCreateInfo createInfo{};
    createInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;
    createInfo.next = _platform->getInstanceCreateInfoAndroid();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.enabledExtensionNames = extensions.data();
    create_info.enabledApiLayerCount = 0;
    create_info.enabledApiLayerNames = nullptr;
    strcpy(create_info.applicationInfo.applicationName, "maverik");
    create_info.aplicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    if (xrCreateInstance(&create_info, &_XRinstance) != XR_SUCCESS) {
        return;
    }
}
