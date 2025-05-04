/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ARenderingContext
*/

#pragma once

#include <vulkan.hpp>

namespace maverik {
    class ARenderingContext {
        public:
            virtual ~ARenderingContext() = default;

            virtual void init() = 0;

        protected:
            VkDevice _logicalDevice;
            VkPhysicalDevice _physicalDevice;
            VkQueue _graphicsQueue;
            VkRenderPass _renderPass;
            VkCommandPool _commandPool;
            VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    };
}
