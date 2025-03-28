/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AndroidPlatform
*/

#include "xr/AndroidPlatform.hpp"

maverik::xr::AndroidPlatform::AndroidPlatform(std::shared_ptr<PlatformData> platformData)
{
    PFN_xrInitializeLoaderKHR initializeLoader = nullptr;

    if (xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&initializeLoader) == XR_SUCCESS) {

        XrLoaderInitInfoAndroidKHR loaderInitInfoAndroid{};
        loaderInitInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
        loaderInitInfoAndroid.next = nullptr;
        loaderInitInfoAndroid.applicationVM = platformData->applicationVM;
        loaderInitInfoAndroid.applicationContext = platformData->applicationActivity;
        initializeLoader(reinterpret_cast<const XrLoaderInitInfoBaseHeaderKHR *>(&loaderInitInfoAndroid));
    }
    _instanceCreateInfoAndroid = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    _instanceCreateInfoAndroid.applicationActivity = platformData->applicationActivity;
    _instanceCreateInfoAndroid.applicationVM = platformData->applicationVM;
}

maverik::xr::AndroidPlatform::~AndroidPlatform()
{
}

