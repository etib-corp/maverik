/*
** ETIB PROJECT, 2025
** maverik
** File description:
** AGraphicalContext
*/

#pragma once

#include "ASwapchainContext.hpp"
#include "ARenderingContext.hpp"

#include <memory>
#include <string>


namespace maverik {
    class AGraphicalContext {
        public:
            virtual ~AGraphicalContext() = default;

            virtual void init() = 0;
            virtual void run() = 0;

            virtual std::vector<std::string> getInstanceExtensions() = 0;

            /**
             * @brief Retrieves the Vulkan context associated with the rendering context.
             *
             * This function provides access to the VulkanContext object, which is
             * encapsulated within the rendering context. The VulkanContext is used
             * for managing Vulkan-specific resources and operations.
             *
             * @return A constant reference to a shared pointer of VulkanContext.
             */
            const std::shared_ptr<VulkanContext>& getVulkanContext() const {
                return _renderingContext->getVulkanContext();
            }

            /**
             * @brief Retrieves the Vulkan instance associated with this graphical context.
             *
             * @return A constant reference to the Vulkan instance (VkInstance).
             */
            const VkInstance& getInstance() const {
                return _instance;
            }

        protected:
            virtual void createInstance() = 0;

            std::shared_ptr<ARenderingContext> _renderingContext;
            std::shared_ptr<ASwapchainContext> _swapchainContext;
            VkInstance _instance = VK_NULL_HANDLE;
    };
}
