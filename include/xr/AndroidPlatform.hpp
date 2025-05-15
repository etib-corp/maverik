/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AndroidPlatform
*/

#pragma once

#include "xr/Openxr-include.hpp"
#include <memory>

struct PlatformData {
    void *applicationVM;
    void *applicationActivity;
};

namespace maverik {
    namespace xr {
        class AndroidPlatform {
            public:
                AndroidPlatform(std::shared_ptr<PlatformData> platformData);
                ~AndroidPlatform();

                /**
                 * @brief Retrieves the Android-specific instance creation information.
                 *
                 * This function returns a pointer to the Android-specific instance creation
                 * structure, cast to a generic XrBaseInStructure pointer. This is typically
                 * used when creating an OpenXR instance on an Android platform.
                 *
                 * @return A pointer to the Android-specific instance creation information
                 *         as an XrBaseInStructure.
                 */
                XrBaseInStructure *getInstanceCreateInfoAndroid() { return reinterpret_cast<XrBaseInStructure *>(&_instanceCreateInfoAndroid); }

            protected:
                XrInstanceCreateInfoAndroidKHR _instanceCreateInfoAndroid{};
            private:
        };

    }
}
