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

namespace maverik {
    namespace xr {
        class Software : public ASoftware {
            public:
                Software(struct android_app *app);
                ~Software();

                void createInstance();

            protected:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                std::shared_ptr<AncroidPlatform> _platform;
            private:
        };

    }
}
