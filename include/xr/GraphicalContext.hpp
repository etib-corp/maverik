/*
** ETIB PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#pragma once

#include "xr/Openxr-include.hpp"
#include "AGraphicalContext.hpp"
#include "xr/RenderingContext.hpp"

namespace maverik {
    namespace xr {
        class GraphicalContext : public maverik::AGraphicalContext {
            public:
                GraphicalContext(XrInstance instance, XrSystemId systemID);
                ~GraphicalContext();

                void init() override;
                void run() override;

                std::vector<std::string> getInstanceExtensions() override;

                void createInstance() override;

            protected:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
            private:
        };
    }
}
