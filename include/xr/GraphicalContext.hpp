/*
** ETIB PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#pragma once

#include "xr/Openxr-include.hpp"
#include "AGraphicalContext.hpp"

namespace maverik {
    namespace xr {
        class GraphicalContext : public maverik::AGraphicalContext {
            public:
                GraphicalContext();
                ~GraphicalContext();

                void init() override;
                void run() override;

                std::vector<std::string> getInstanceExtensions() override;

                void createInstance() override;

                void setXRInstance(XrInstance instance) { _XRinstance = instance; }
                void setXRSystemID(XrSystemId systemID) { _XRsystemID = systemID; }

            protected:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
            private:
        };
    }
}
