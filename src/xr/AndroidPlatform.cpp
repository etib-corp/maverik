/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AndroidPlatform
*/

#include "xr/AndroidPlatform.hpp"

maverik::xr::AndroidPlatform::AndroidPlatform(struct android_app *app)
{
    PFN_xrInitializeLoaderKHR initializeLoader = nullptr;

    if (xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&initializeLoader) == XR_SUCCESS) {

        XrLoaderInitInfoAndroidKHR loaderInitInfoAndroid{};
        loaderInitInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR
        loaderInitInfoAndroid.next = nullptr;
        loaderInitInfoAndroid.applicationVM = app->activity->vm;
        loaderInitInfoAndroid.applicationContext = app->activity->clazz;
        initialize_loader(reinterpret_cast<const XrLoaderInitInfoBaseHeaderKHR *>(&loader_init_info_android));
    }
    _instanceCreateInfoAndroid = {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    _instanceCreateInfoAndroid.applicationActivity = app->activity->clazz;
    _instanceCreateInfoAndroid.applicationVM = app->activity->vm;
}

maverik::xr::AndroidPlatform::~AndroidPlatform()
{
}

