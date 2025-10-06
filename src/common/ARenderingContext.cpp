/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ARenderingContext
*/

#include "ARenderingContext.hpp"

///////////////////////
// Protected methods //
///////////////////////

/**
 * @brief Determines the maximum usable sample count for multisampling.
 *
 * This function queries the physical device properties to determine the
 * maximum sample count that can be used for both color and depth
 * framebuffer attachments. It checks the supported sample counts in
 * descending order of quality (from 64x to 1x) and returns the highest
 * supported sample count.
 *
 * @return VkSampleCountFlagBits The maximum usable sample count supported
 *         by the physical device.
 */
VkSampleCountFlagBits maverik::ARenderingContext::getMaxUsableSampleCount() const
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(_physicalDevice, &physicalDeviceProperties);
    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
    if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
    if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
    if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
    if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
    if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

void maverik::ARenderingContext::createCommandPool()
{
    if (_logicalDevice == VK_NULL_HANDLE) {
        std::cerr << "Logical device is not initialized" << std::endl;
        return;
    }

    if (_commandPool != VK_NULL_HANDLE) {
        return;
    }

    Utils::QueueFamilyIndices queueCreateInfo = Utils::findQueueFamilies(_physicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueCreateInfo.graphicsFamily.value();
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(_logicalDevice, &commandPoolCreateInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan command pool" << std::endl;
        return;
    }
}
