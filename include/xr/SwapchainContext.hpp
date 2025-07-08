/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#ifndef SWAPCHAINCONTEXT_HPP_
#define SWAPCHAINCONTEXT_HPP_


#include "ASwapchainContext.hpp"
#include "Utils.hpp"
#include "xr/Openxr-include.hpp"


namespace maverik {
    namespace xr {

        struct SwapchainContextCreationPropertiesXR {
            XrInstance _instance;
            XrSystemId _systemId;
            XrSession _session;
        };

        class SwapchainContext : public ASwapchainContext {
            public:
                SwapchainContext(const SwapchainContextCreationPropertiesXR& properties);
                ~SwapchainContext();

            protected:
                void init();

                uint64_t selectSwapchainFormat(const std::vector<int64_t> &swapchainFormats);

            private:
                XrInstance _instance;
                XrSystemId _systemId;
                XrSession _session;

                std::vector<XrViewConfigurationView> _viewsConfigurations;
                std::vector<XrView> _views;
        };

    }
}

#endif /* !SWAPCHAINCONTEXT_HPP_ */
