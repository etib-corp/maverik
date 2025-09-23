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
