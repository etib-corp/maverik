/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Utils
*/

#include "vk/Utils.hpp"

/**
* @brief Queries the swap chain support details for a given physical device and surface.
*
* This function retrieves the capabilities, supported surface formats, and present modes
* of the swap chain for the specified Vulkan physical device and surface. The results
* are stored in a SwapChainSupportDetails structure.
*
* @param device The Vulkan physical device to query.
* @param surface The Vulkan surface to query.
* @return A SwapChainSupportDetails structure containing the swap chain support details.
*
* The returned structure includes:
* - Capabilities: The surface capabilities (e.g., min/max image count, current extent, etc.).
* - Formats: A list of supported surface formats (e.g., pixel format and color space).
* - Present Modes: A list of supported presentation modes (e.g., FIFO, Mailbox, etc.).
*/
maverik::vk::Utils::SwapChainSupportDetails maverik::vk::Utils::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;
    uint32_t formatCount;
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    return details;
}

/**
* @brief Finds the queue families that support specific operations on a given Vulkan physical device and surface.
*
* This function identifies the queue families that support graphics operations and presentation to a given surface.
* It iterates through the queue families of the specified physical device and checks their capabilities.
*
* @param device The Vulkan physical device to query for queue family properties.
* @param surface The Vulkan surface to check for presentation support.
* @return QueueFamilyIndices A structure containing the indices of the graphics and presentation queue families.
*         If no suitable queue families are found, the indices will remain incomplete.
*/
maverik::vk::Utils::QueueFamilyIndices maverik::vk::Utils::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

/**
* @brief Finds a suitable memory type for a Vulkan resource.
*
* This function searches through the memory types available on the given physical device
* and returns the index of a memory type that satisfies the specified type filter and
* memory property flags.
*
* @param physicalDevice The Vulkan physical device to query for memory properties.
* @param typeFilter A bitmask specifying the acceptable memory types. Each bit represents
*                   a memory type, and the function will check which types are suitable.
* @param properties A set of memory property flags that the desired memory type must have.
*                   For example, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT or 
*                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
* @return The index of a suitable memory type.
* @throws std::runtime_error If no suitable memory type is found.
*/
uint32_t maverik::vk::Utils::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

/**
* @brief Finds a suitable depth format for a Vulkan physical device.
*
* This function determines the most appropriate depth format supported by the given
* Vulkan physical device. It checks a list of candidate formats in order of preference
* and ensures that the selected format supports the specified tiling and feature requirements.
*
* @param physicalDevice The Vulkan physical device to query for supported formats.
* @return VkFormat The selected depth format that meets the requirements.
*
* The function prioritizes the following formats (in order):
* - VK_FORMAT_D32_SFLOAT
* - VK_FORMAT_D32_SFLOAT_S8_UINT
* - VK_FORMAT_D24_UNORM_S8_UINT
*
* The selected format must support:
* - VK_IMAGE_TILING_OPTIMAL
* - VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
*/
VkFormat maverik::vk::Utils::findDepthFormat(VkPhysicalDevice physicalDevice)
{
    return findSupportedFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

/**
* @brief Creates a Vulkan image and allocates memory for it.
*
* This function initializes a Vulkan image with the specified parameters, allocates
* the required memory, and binds the memory to the image. It is a utility function
* to simplify Vulkan image creation.
*
* @param logicalDevice The Vulkan logical device used to create the image.
* @param physicalDevice The Vulkan physical device used to find memory properties.
* @param width The width of the image in pixels.
* @param height The height of the image in pixels.
* @param mipLevels The number of mipmap levels for the image.
* @param numSamples The number of samples per pixel for multisampling.
* @param format The format of the image (e.g., VK_FORMAT_R8G8B8A8_SRGB).
* @param tiling The tiling arrangement of the image (e.g., VK_IMAGE_TILING_OPTIMAL).
* @param usage The intended usage of the image (e.g., VK_IMAGE_USAGE_SAMPLED_BIT).
* @param properties The memory properties required for the image (e.g., VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).
* @param image A reference to a VkImage handle where the created image will be stored.
* @param imageMemory A reference to a VkDeviceMemory handle where the allocated memory will be stored.
*
* @throws std::runtime_error If the image creation or memory allocation fails.
*/
void maverik::vk::Utils::createImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.mipLevels = mipLevels;
    imageInfo.samples = numSamples;

    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Utils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate image memory!");
    }

    vkBindImageMemory(logicalDevice, image, imageMemory, 0);
}

/**
* @brief Transitions the layout of a Vulkan image.
*
* This function is used to transition a Vulkan image from one layout to another.
* It sets up the necessary image memory barriers and pipeline stages to ensure
* proper synchronization during the transition.
*
* @param logicalDevice The Vulkan logical device.
* @param commandPool The command pool used to allocate the command buffer.
* @param graphicsQueue The graphics queue used to submit the command buffer.
* @param image The Vulkan image to transition.
* @param format The format of the image, used to determine if it has a stencil component.
* @param oldLayout The current layout of the image.
* @param newLayout The desired layout of the image.
* @param mipLevels The number of mipmap levels in the image.
*
* @throws std::invalid_argument If the layout transition is unsupported.
*
* @note This function assumes that the image is not being used concurrently
*       by other operations during the transition.
*/
void maverik::vk::Utils::transitionImageLayout(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(logicalDevice, commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = mipLevels;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (Utils::hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    Utils::endSingleTimeCommands(logicalDevice, commandPool, graphicsQueue, commandBuffer);
}

bool maverik::vk::Utils::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions)
{
    QueueFamilyIndices indices = Utils::findQueueFamilies(device, surface);
    bool extensionsSupported = Utils::checkDeviceExtensionSupport(device, deviceExtensions);
    bool swapChainAdequate = false;

    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = Utils::querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

void maverik::vk::Utils::createBuffer(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Utils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

void maverik::vk::Utils::copyBufferToImage(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(logicalDevice, commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    Utils::endSingleTimeCommands(logicalDevice, commandPool, graphicsQueue, commandBuffer);
}

void maverik::vk::Utils::generateMipmaps(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = Utils::beginSingleTimeCommands(logicalDevice, commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    Utils::endSingleTimeCommands(logicalDevice, commandPool, graphicsQueue, commandBuffer);
}

/////////////////////
// Private methods //
/////////////////////

/**
* @brief Finds a supported Vulkan format from a list of candidates based on specified tiling and feature requirements.
*
* This function iterates through a list of candidate formats and checks if any of them
* meet the specified requirements for image tiling and format features. If a suitable
* format is found, it is returned. If no suitable format is found, an exception is thrown.
*
* @param physicalDevice The Vulkan physical device to query for format support.
* @param candidates A list of candidate VkFormat values to check for support.
* @param tiling The desired VkImageTiling (e.g., VK_IMAGE_TILING_LINEAR or VK_IMAGE_TILING_OPTIMAL).
* @param features The required VkFormatFeatureFlags that the format must support.
*
* @return VkFormat The first format from the candidates list that meets the specified requirements.
*
* @throws std::runtime_error If no supported format is found in the candidates list.
*/
VkFormat maverik::vk::Utils::findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

/**
* @brief Checks if the given Vulkan format includes a stencil component.
*
* This function determines whether the specified Vulkan format contains
* a stencil component by comparing it against known formats that include
* stencil data.
*
* @param format The Vulkan format to check (VkFormat).
* @return true if the format includes a stencil component, false otherwise.
*/
bool maverik::vk::Utils::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

/**
* @brief Begins a single-time command buffer for Vulkan operations.
*
* This function allocates and begins recording a command buffer that is intended
* for short-lived operations, such as resource transfers or one-off commands.
* The command buffer is allocated from the specified command pool and is configured
* with the `VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT` flag, indicating that it
* will be submitted only once before being reset or freed.
*
* @param logicalDevice The Vulkan logical device used to allocate the command buffer.
* @param commandPool The command pool from which the command buffer will be allocated.
* @return VkCommandBuffer The allocated and begun command buffer ready for recording commands.
*
* @note The caller is responsible for ending the command buffer recording and submitting
*       it to a queue, as well as cleaning up resources after use.
*/
VkCommandBuffer maverik::vk::Utils::beginSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

/**
 * @brief Ends a single-time command buffer operation and cleans up resources.
 *
 * This function finalizes the execution of a single-time command buffer by
 * submitting it to the specified graphics queue, waiting for the queue to
 * become idle, and then freeing the command buffer resources.
 *
 * @param logicalDevice The Vulkan logical device used to free the command buffer.
 * @param commandPool The command pool from which the command buffer was allocated.
 * @param graphicsQueue The Vulkan queue to which the command buffer is submitted.
 * @param commandBuffer The command buffer to be ended, submitted, and freed.
 */
void maverik::vk::Utils::endSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

bool maverik::vk::Utils::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensions)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}
