/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Software
*/

#pragma once

#include "ASoftware.hpp"
#include "xr/Openxr-include.hpp"
#include "xr/GraphicalContext.hpp"
#include "xr/AndroidPlatform.hpp"

#include <algorithm>
#include <cstring>

namespace maverik {
    namespace xr {
        class Software : public ASoftware {
            public:
                Software(std::shared_ptr<PlatformData> platformData);
                ~Software();

                void createInstance();

            protected:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
                std::shared_ptr<AndroidPlatform> _platform;
            private:
        };

    }
}
