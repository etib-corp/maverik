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

                XrBaseInStructure *getInstanceCreateInfoAndroid() { return reinterpret_cast<XrBaseInStructure *>(&_instanceCreateInfoAndroid); }

            protected:
                XrInstanceCreateInfoAndroidKHR _instanceCreateInfoAndroid{};
            private:
        };

    }
}
