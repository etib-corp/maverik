/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ARenderingContext
*/

#pragma once

#include <vulkan.hpp>

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

            const std::shared_ptr<VulkanContext>& getVulkanContext() const {
                return _vulkanContext;
            }

        protected:
            VkDevice _logicalDevice;
            VkPhysicalDevice _physicalDevice;
            VkQueue _graphicsQueue;
            VkRenderPass _renderPass;
            VkCommandPool _commandPool;
            VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

            std::shared_ptr<VulkanContext> _vulkanContext;
    };
}
