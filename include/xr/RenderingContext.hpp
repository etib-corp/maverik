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
        class RenderingContext : plublic maverik::ARenderingContext {
            public:
                RenderingContext(XrInstance instance, XrSystemId systemID);
                ~RenderingContext() override;

                void init() override;

            protected:
            private:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
        };
    }
}


#endif /* !RENDERINGCONTEXT_HPP_ */
