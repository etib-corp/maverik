/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#pragma once

#include "xr/Openxr-include.hpp"
#include "ARenderingContext.hpp"

namespace maverik {
    namespace xr {
        class RenderingContext : public maverik::ARenderingContext {
            public:
                RenderingContext(XrInstance XRinstance, VkInstance instance,  XrSystemId systemID);
                ~RenderingContext() override;

                void init() override;

            protected:
            private:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
                VkInstance _vulkanInstance = VK_NULL_HANDLE;
        };
    }
}


#endif /* !RENDERINGCONTEXT_HPP_ */
