/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ARenderingContext
*/

#pragma once

#include <vulkan.hpp>
#include <memory>

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
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    uint32_t graphicsQueueFamilyIndex;
};


namespace maverik {
    class ARenderingContext {
        public:
            virtual ~ARenderingContext() = default;

            virtual void init() = 0;

            virtual void pickPhysicalDevice(VkInstance instance) = 0;

            virtual void createLogicalDevice() = 0;

            virtual void createCommandPool() = 0;

            virtual void createRenderPass() = 0;

            /**
             * @brief Retrieves the Vulkan context associated with this rendering context.
             *
             * @return A constant reference to a shared pointer of VulkanContext.
             */
            const std::shared_ptr<VulkanContext>& getVulkanContext() const {
                return _vulkanContext;
            }

        protected:

            VkSampleCountFlagBits getMaxUsableSampleCount() const;

            VkDevice _logicalDevice;
            VkPhysicalDevice _physicalDevice;
            VkQueue _graphicsQueue;
            VkCommandPool _commandPool;
            VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

            std::shared_ptr<VulkanContext> _vulkanContext;
    };
}
