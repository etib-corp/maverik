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

        /**
         * @struct GraphicalContextProperties
         * @brief Represents the properties required for initializing a graphical context in an XR environment.
         *
         * This structure contains essential information for setting up and managing a graphical context,
         * including the XR instance, XR system ID, and a shared rendering context.
         */
        struct GraphicalContextPropertiesXR {
            /**
             * @brief The XR instance handle used for interacting with the XR runtime.
             */
            XrInstance _XRinstance;

            /**
             * @brief The XR system ID representing the specific XR system being used.
             */
            XrSystemId _XRsystemID;
        };

        class GraphicalContext : public maverik::AGraphicalContext {
            public:

                GraphicalContext(const GraphicalContextPropertiesXR &properties);
                ~GraphicalContext();

                void init() override;
                void run() override;

                std::vector<std::string> getInstanceExtensions() override;


            private:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
        };
    }
}
