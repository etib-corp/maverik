/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AndroidPlatform
*/

#pragma once

#include "xr/Openxr-include.hpp"

namespace maverik {
    namespace xr {
        class AndroidPlatform {
            public:
                AndroidPlatform(struct android_app *app);
                ~AndroidPlatform();

                XrBaseInStructure *getInstanceCreateInfoAndroid() { return reinterpret_cast<XrBaseInStructure *>(&_instanceCreateInfoAndroid); }

            protected:
                XrInstanceCreateInfoAndroidKHR _instanceCreateInfoAndroid{};
            private:
        };

    }
}
