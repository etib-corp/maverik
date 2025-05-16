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
#include <set>
#include <fstream>

#include <vulkan/vulkan.h>

namespace maverik {
    namespace vk {
        class Utils {
            public:
                virtual ~Utils() = default;

                // Structs

                /**
                * @brief Struct to encapsulate details about swap chain support.
                *
                * This struct contains information about the swap chain capabilities,
                * available surface formats, and present modes for a Vulkan surface.
                * It is typically used to query and store the swap chain support details
                * for a Vulkan physical device.
                *
                * @struct SwapChainSupportDetails
                */
                struct SwapChainSupportDetails {
                    /*
                    * @brief Vulkan surface capabilities, such as the minimum and maximum image
                    * count, extent, and supported transforms.
                    */
                    VkSurfaceCapabilitiesKHR capabilities;

                    /*
                    * @brief A list of supported surface formats (color space and pixel format).
                    */
                    std::vector<VkSurfaceFormatKHR> formats;

                    /*
                    * @brief A list of supported presentation modes (e.g., FIFO, Mailbox, etc.).
                    */
                    std::vector<VkPresentModeKHR> presentModes;
                };

                /**
                * @brief A structure to represent Vulkan queue family indices.
                *
                * This structure is used to store the indices of queue families
                * that support specific operations, such as graphics and presentation.
                *
                */
                struct QueueFamilyIndices {
                    /*
                    * An optional value representing the index of the queue family
                    * that supports graphics operations.
                    */
                    std::optional<uint32_t> graphicsFamily;

                    /*
                    * An optional value representing the index of the queue family
                    * that supports presentation operations.
                    */
                    std::optional<uint32_t> presentFamily;

                    /*
                    * @brief isComplete
                    *
                    * A method to check if both graphicsFamily and presentFamily
                    * have been assigned values.
                    *
                    * @return true if both graphicsFamily and presentFamily have values,
                    * indicating that the required queue families are available. False otherwise.
                    */
                    bool isComplete() {
                        return graphicsFamily.has_value() && presentFamily.has_value();
                    }
                };

                static std::vector<char> readFile(const std::string& filename);

                static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

                static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
                static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
                static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);

                static void createImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

                static void transitionImageLayout(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

                static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions);

                static void createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

                static void copyBufferToImage(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

                static void generateMipmaps(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

                static void copyBuffer(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

                static VkShaderModule createShaderModule(VkDevice logicalDevice, const std::vector<char>& code);

                static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);

                static VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

            private:
                static VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

                static bool hasStencilComponent(VkFormat format);

                static VkCommandBuffer beginSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool);
                static void endSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer);

                static bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions);

        };
    }
}

#endif /* !UTILS_HPP_ */
