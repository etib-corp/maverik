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


                /**
                 * @brief Initializes the Vulkan rendering context for the XR application.
                 *
                 * This function sets up the Vulkan graphics device, logical device, and associated
                 * resources required for rendering in an XR environment. It retrieves necessary
                 * Vulkan-related functions from the XR runtime, creates a Vulkan device, and
                 * initializes the Vulkan context with the created resources.
                 *
                 * @details
                 * - Retrieves the `xrGetVulkanGraphicsDevice2KHR` and `xrCreateVulkanDeviceKHR`
                 *   functions from the XR runtime.
                 * - Queries the Vulkan physical device using the XR runtime.
                 * - Creates a Vulkan logical device with a graphics queue.
                 * - Retrieves the graphics queue and creates a command pool for command buffer management.
                 * - Updates the Vulkan context with the initialized Vulkan resources.
                 *
                 * @note
                 * - The function will return early if the XR instance handle (`_XRinstance`) is invalid.
                 * - If any Vulkan or XR function call fails, an error message is logged, and the function exits.
                 *
                 * @warning
                 * - Ensure that `_XRinstance`, `_XRsystemID`, and `_vulkanInstance` are properly initialized
                 *   before calling this function.
                 * - This function assumes that the Vulkan instance and XR runtime are compatible.
                 *
                 * @remarks
                 * - The Vulkan context (`_vulkanContext`) is updated with the logical device, physical device,
                 *   graphics queue, command pool, and queue family index.
                 * - The function does not handle Vulkan extensions or validation layers explicitly.
                 */
                void init() override;

            protected:
            private:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
                VkInstance _vulkanInstance = VK_NULL_HANDLE;
        };
    }
}
