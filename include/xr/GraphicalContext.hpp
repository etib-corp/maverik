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

                /**
                 * @brief Initializes the graphical context.
                 *
                 * This function initializes the graphical context by creating a Vulkan
                 * instance and setting up the rendering context. It should be called
                 * before using the graphical context for rendering operations.
                 */
                void init() override;

                void run() override;

                /**
                 * @brief Retrieves the Vulkan instance extensions required for the
                 *        graphical context.
                 *
                 * This function returns a vector of strings representing the Vulkan
                 * instance extensions required for the graphical context. The returned
                 * extensions are used during the creation of the Vulkan instance.
                 *
                 * @return A vector of strings containing the Vulkan instance extensions.
                 */
                std::vector<std::string> getInstanceExtensions() override;


                protected:
                /**
                 * @brief Creates a Vulkan instance for the graphical context using OpenXR.
                 *
                 * This function initializes a Vulkan instance required for rendering in the
                 * graphical context. It retrieves the necessary Vulkan graphics requirements
                 * and function pointers from the OpenXR runtime, sets up Vulkan instance
                 * creation parameters, and creates the Vulkan instance. If any step fails,
                 * an error message is logged, and the function returns early.
                 *
                 * The created Vulkan instance is used to initialize a rendering context
                 * for further rendering operations.
                 *
                 * @details
                 * - Retrieves the `xrGetVulkanGraphicsRequirements2KHR` and
                 *   `xrCreateVulkanInstanceKHR` function pointers from the OpenXR runtime.
                 * - Queries Vulkan graphics requirements for the current OpenXR system.
                 * - Configures Vulkan instance creation parameters, including application
                 *   info, layers, and extensions.
                 * - Uses the `xrCreateVulkanInstanceKHR` function to create the Vulkan instance.
                 * - Initializes a rendering context (`maverik::xr::RenderingContext`) with
                 *   the created Vulkan instance and OpenXR system ID.
                 *
                 * @note This function assumes that `_XRinstance` and `_XRsystemID` are
                 *       already initialized and valid.
                 *
                 * @warning If Vulkan instance creation fails, the function logs an error
                 *          and does not proceed with rendering context initialization.
                 */
                void createInstance() override;

                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
            private:
        };
    }
}
