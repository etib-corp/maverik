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
                SwapchainContext(XrInstance instance, XrSystemId systemID, XrSession session);
                ~SwapchainContext();

                void init();

                protected:
                uint64_t selectSwapchainFormat(const std::vector<int64_t> &swapchainFormats);

                XrInstance _instance = XR_NULL_HANDLE;
                XrSession _session = XR_NULL_HANDLE;
                XrSystemId _systemID = XR_NULL_SYSTEM_ID;
                std::vector<XrViewConfigurationView> _viewsConfigurations;
                std::vector<XrView> _views;
            private:
        };
    }
}

