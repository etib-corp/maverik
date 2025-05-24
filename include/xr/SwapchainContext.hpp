/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#pragma once

#include "ASwapchainContext.hpp"

#include "xr/Openxr-include.hpp"

namespace maverik {
    namespace xr {
        class SwapchainContext : public ASwapchainContext {
            public:
                SwapchainContext(XrInstance instance, XrSession session);
                ~SwapchainContext();

                void init();

            protected:
                XrInstance _instance = XR_NULL_HANDLE;
                XrSession _session = XR_NULL_HANDLE;
            private:
        };
    }
}

