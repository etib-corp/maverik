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

                /**
                 * @brief Constructor for the Software class.
                 *
                 * Initializes the Software instance with the provided platform data.
                 * Creates an AndroidPlatform instance using the given platform data and sets it as the platform.
                 * The graphical context is initialized to nullptr.
                 *
                 * @param platformData A shared pointer to the PlatformData object containing platform-specific information.
                 */
                Software(const std::shared_ptr<PlatformData> &platformData);
                ~Software();


            protected:
                /**
                 * @brief Creates an OpenXR instance, initializes the graphical context, and sets up an XR session.
                 *
                 * This function performs the following steps:
                 * 1. Checks if the XR instance is already created. If so, it returns immediately.
                 * 2. Prepares a list of required extensions, including platform-specific and graphical context extensions.
                 * 3. Fills out the XrInstanceCreateInfo structure and creates the XR instance.
                 * 4. Retrieves the XR system ID for the head-mounted display form factor.
                 * 5. Initializes the graphical context and retrieves the Vulkan context.
                 * 6. Sets up the Vulkan graphics binding for OpenXR.
                 * 7. Creates an XR session using the initialized XR instance and system ID.
                 *
                 * @note If any step fails, an error message is logged to std::cerr, and the function returns without completing the remaining steps.
                 *
                 */
                void createInstance();

                void initializeSystem();

                void initializeSession();

                void createVisualizedSpace();

                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
                XrSession _XRsession = XR_NULL_HANDLE;
                std::shared_ptr<AndroidPlatform> _platform;
                std::vector<XrSpace> _XRvisualizedSpaces;
            private:
        };

    }
}
