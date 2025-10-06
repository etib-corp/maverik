/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ARenderingContext
*/

#pragma once

#include "maverik.hpp"
#include <memory>
#include <array>

#include "Utils.hpp"

/**
 * @struct VulkanContext
 * @brief Represents the Vulkan rendering context, encapsulating essential Vulkan objects and configurations.
 *
 * This structure is used to manage and store Vulkan-specific resources required for rendering operations.
 *
 * @var VulkanContext::logicalDevice
 * The Vulkan logical device used for rendering operations.
 *
 * @var VulkanContext::physicalDevice
 * The Vulkan physical device representing the GPU hardware.
 *
 * @var VulkanContext::graphicsQueue
 * The Vulkan queue used for submitting graphics commands.
 *
 * @var VulkanContext::renderPass
 * The Vulkan render pass object defining the rendering pipeline.
 *
 * @var VulkanContext::commandPool
 * The Vulkan command pool used for allocating command buffers.
 *
 * @var VulkanContext::graphicsQueueFamilyIndex
 * The index of the queue family that supports graphics operations.
 */
struct  VulkanContext{
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkCommandPool commandPool;
    uint32_t graphicsQueueFamilyIndex;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
};

namespace maverik {
    class ARenderingContext {
        public:
            virtual ~ARenderingContext() = default;

            virtual void init() = 0;

            /**
             * @brief Retrieves the Vulkan context associated with this rendering context.
             *
             * @return A constant reference to a shared pointer of VulkanContext.
             */
            const std::shared_ptr<VulkanContext>& getVulkanContext() const {
                return _vulkanContext;
            }

        private:
            /**
             * @brief Chooses the most suitable surface format for the swapchain.
             *
             * This function iterates through the list of available surface formats and
             * selects the one that best matches the desired format and color space.
             * If the preferred format (VK_FORMAT_B8G8R8A8_SRGB) and color space
             * (VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) are available, it returns that format.
             * Otherwise, it defaults to the first available format in the list.
             *
             * @param availableFormats A vector of VkSurfaceFormatKHR structures representing
             *                         the formats supported by the surface.
             * @return VkSurfaceFormatKHR The chosen surface format.
             */
            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
            {
                for (const auto& availableFormat : availableFormats) {
                    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        return availableFormat;
                    }
                }
                return availableFormats[0];
            }

        protected:

            virtual void pickPhysicalDevice(VkInstance instance) = 0;

            virtual void createLogicalDevice() = 0;

            virtual void createCommandPool();

            VkSampleCountFlagBits getMaxUsableSampleCount() const;

            VkDevice _logicalDevice;
            VkPhysicalDevice _physicalDevice;
            VkQueue _graphicsQueue;
            VkCommandPool _commandPool;
            VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

            std::shared_ptr<VulkanContext> _vulkanContext;

    };
}
