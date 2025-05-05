/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Utils
*/

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

namespace maverik {
    namespace vk {
        class Utils {
            public:
                virtual ~Utils() = default;

                struct SwapChainSupportDetails {
                    VkSurfaceCapabilitiesKHR capabilities;
                    std::vector<VkSurfaceFormatKHR> formats;
                    std::vector<VkPresentModeKHR> presentModes;
                };

                struct QueueFamilyIndices {
                    std::optional<uint32_t> graphicsFamily;
                    std::optional<uint32_t> presentFamily;

                    bool isComplete() {
                        return graphicsFamily.has_value() && presentFamily.has_value();
                    }
                };

                static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

                static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
                static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
                static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

                static void createImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

                static void transitionImageLayout(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

            private:
                static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

                static bool hasStencilComponent(VkFormat format);

                static VkCommandBuffer beginSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool);
                static void endSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer);

        };
    }
}

#endif /* !UTILS_HPP_ */
