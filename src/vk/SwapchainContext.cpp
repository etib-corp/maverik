/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#include "vk/SwapchainContext.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

////////////////////
// Public methods //
////////////////////

/**
 * @brief Constructs a SwapchainContext object and initializes Vulkan resources.
 *
 * This constructor sets up the swapchain context by initializing the Vulkan surface,
 * physical device, logical device, and window. It also creates the necessary resources
 * for rendering, including color resources, depth resources, and framebuffers.
 *
 * @param surface The Vulkan surface handle associated with the swapchain.
 * @param physicalDevice The Vulkan physical device used for rendering.
 * @param logicalDevice The Vulkan logical device used for rendering.
 * @param window A pointer to the GLFW window associated with the swapchain.
 * @param msaaSamples The number of samples for multisample anti-aliasing (MSAA).
 * @param commandPool The Vulkan command pool used for command buffer allocation.
 * @param graphicsQueue The Vulkan graphics queue used for rendering commands.
 * @param renderPass The Vulkan render pass used for rendering operations.
 */
maverik::vk::SwapchainContext::SwapchainContext(const SwapchainContextCreationProperties& properties)
{
    struct TextureImageCreationProperties textureImageProperties = {
        properties._physicalDevice,
        properties._logicalDevice,
        properties._commandPool,
        properties._msaaSamples,
        properties._graphicsQueue
    };

    this->init(properties._surface, properties._physicalDevice, properties._logicalDevice, properties._window);

    this->createColorResources(properties._logicalDevice, properties._physicalDevice, properties._msaaSamples);
    this->createDepthResources(textureImageProperties);
    this->createFramebuffers(properties._logicalDevice, properties._renderPass);
}

/**
 * @brief Destructor for the SwapchainContext class.
 *
 * Cleans up Vulkan resources associated with the swapchain context.
 */
maverik::vk::SwapchainContext::~SwapchainContext()
{
}

/**
 * @brief Recreates the Vulkan swapchain and associated resources.
 *
 * This function handles the recreation of the swapchain and its dependent resources
 * when the window is resized or other conditions require a swapchain update. It ensures
 * that the Vulkan device is idle before performing cleanup and reinitialization.
 *
 * @param surface The Vulkan surface associated with the swapchain.
 * @param physicalDevice The Vulkan physical device used for resource creation.
 * @param logicalDevice The Vulkan logical device used for operations.
 * @param window The GLFW window handle associated with the application.
 * @param msaaSamples The number of samples used for multisampling (MSAA).
 * @param commandPool The Vulkan command pool used for command buffer allocation.
 * @param graphicsQueue The Vulkan graphics queue used for rendering operations.
 * @param renderPass The Vulkan render pass used for rendering.
 */
void maverik::vk::SwapchainContext::recreate(const SwapchainContextCreationProperties& properties)
{
    int width = 0;
    int height = 0;
    struct TextureImageCreationProperties textureImageProperties = {
        properties._physicalDevice,
        properties._logicalDevice,
        properties._commandPool,
        properties._msaaSamples,
        properties._graphicsQueue
    };

    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(properties._window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(properties._logicalDevice);

    this->cleanup(properties._logicalDevice);

    this->init(properties._surface, properties._physicalDevice, properties._logicalDevice, properties._window);

    this->createColorResources(properties._logicalDevice, properties._physicalDevice, properties._msaaSamples);
    this->createDepthResources(textureImageProperties);
    this->createFramebuffers(properties._logicalDevice, properties._renderPass);
}

///////////////////////
// Protected methods //
///////////////////////

/**
 * @brief Initializes the Vulkan swapchain context.
 *
 * This function sets up the Vulkan swapchain, which is responsible for managing
 * the images that are presented to the screen. It configures the swapchain based
 * on the provided surface, physical device, logical device, and window, and creates
 * the necessary image views for rendering.
 *
 * @param surface The Vulkan surface to present images to.
 * @param physicalDevice The Vulkan physical device (GPU) to use.
 * @param logicalDevice The Vulkan logical device associated with the physical device.
 * @param window The GLFW window handle used to determine the swapchain extent.
 *
 * @throws std::runtime_error If the swapchain creation fails.
 */
void maverik::vk::SwapchainContext::init(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *window)
{
    Utils::SwapChainSupportDetails swapChainSupport = Utils::querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = this->chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = this->chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = this->chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Utils::QueueFamilyIndices indices = Utils::findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &_swapchain.swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(logicalDevice, _swapchain.swapchain, &imageCount, nullptr);
    _swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, _swapchain.swapchain, &imageCount, _swapchainImages.data());

    _swapchainFormat = surfaceFormat.format;
    _swapchainExtent = extent;

    this->createImageViews(logicalDevice);
}

/**
 * @brief Creates image views for all swapchain images.
 *
 * This function resizes the `_imageViews` vector to match the size of the `_swapchainImages` vector
 * and initializes each image view by calling the `createImageView` method. The image views are
 * created with the specified swapchain format and color aspect, and are associated with the provided
 * logical Vulkan device.
 *
 * @param logicalDevice The Vulkan logical device used to create the image views.
 */
void maverik::vk::SwapchainContext::createImageViews(VkDevice logicalDevice)
{
    _imageViews.resize(_swapchainImages.size());

    for (uint32_t i = 0; i < _swapchainImages.size(); i++) {
        _imageViews[i] = this->createImageView(_swapchainImages[i], _swapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, logicalDevice);
    }
}

/**
 * @brief Creates framebuffers for the swapchain images.
 *
 * This function initializes a framebuffer for each image view in the swapchain.
 * Each framebuffer is configured with a color attachment, a depth attachment,
 * and the corresponding swapchain image view. The framebuffers are stored in
 * the `_swapchainFramebuffers` member variable.
 *
 * @param renderPass The Vulkan render pass to be used with the framebuffers.
 * @param logicalDevice The Vulkan logical device used to create the framebuffers.
 *
 * @throws std::runtime_error If framebuffer creation fails.
 */
void maverik::vk::SwapchainContext::createFramebuffers(VkDevice logicalDevice, VkRenderPass renderPass)
{
    _swapchainFramebuffers.resize(_imageViews.size());

    for (size_t i = 0; i < _imageViews.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            _colorImageView,
            _depthImageView,
            _imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = _swapchainExtent.width;
        framebufferInfo.height = _swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &_swapchainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

/**
 * @brief Cleans up Vulkan resources associated with the swapchain context.
 *
 * This function destroys all framebuffers, image views, and the swapchain
 * associated with the swapchain context. It ensures proper resource
 * deallocation to prevent memory leaks.
 *
 * @param logicalDevice The Vulkan logical device used to destroy the resources.
 */
void maverik::vk::SwapchainContext::cleanup(VkDevice logicalDevice)
{
    for (auto framebuffer : _swapchainFramebuffers) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }

    for (auto imageView : _imageViews) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, _swapchain.swapchain, nullptr);
}

/**
 * @brief Creates a Vulkan texture image from a file and prepares it for use in shaders.
 *
 * Loads an image from the specified file path, uploads it to a Vulkan image resource,
 * generates mipmaps, and transitions the image layout for shader access.
 *
 * @param physicalDevice The Vulkan physical device used for memory property queries.
 * @param logicalDevice The Vulkan logical device used for resource creation.
 * @param commandPool The command pool used for submitting transfer and layout transition commands.
 * @param graphicsQueue The graphics queue used to execute transfer and layout transition commands.
 * @param texturePath The file path to the texture image to be loaded.
 *
 * @throws std::runtime_error If the texture image fails to load.
 *
 * @note The function creates a staging buffer for image data transfer, allocates and fills
 *       device-local image memory, generates mipmaps, and cleans up temporary resources.
 */
void maverik::vk::SwapchainContext::createTextureImage(const std::string& texturePath, const TextureImageCreationProperties& properties)
{
    int texWidth = 0;
    int texHeight = 0;
    int texChannels = 0;
    stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image !");
    }

    _mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    Utils::CreateBufferProperties stagingBufferProperties = {
        ._logicalDevice = properties._logicalDevice,
        ._physicalDevice = properties._physicalDevice,
        ._size = imageSize,
        ._usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        ._buffer = stagingBuffer,
        ._bufferMemory = stagingBufferMemory
    };

    Utils::createBuffer(stagingBufferProperties);

    void* data;
    vkMapMemory(properties._logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(properties._logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    Utils::CreateImageProperties imageProperties = {
        ._logicalDevice = properties._logicalDevice,
        ._physicalDevice = properties._physicalDevice,
        ._width = (uint32_t)texWidth,
        ._height = (uint32_t)texHeight,
        ._mipLevels = _mipLevels,
        ._format = VK_FORMAT_R8G8B8A8_SRGB,
        ._tiling = VK_IMAGE_TILING_OPTIMAL,
        ._usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        ._image = _textureImage[texturePath],
        ._imageMemory = _textureImageMemory
    };
    Utils::createImage(imageProperties);

    Utils::TransitionImageLayoutProperties transitionProperties = {
        ._logicalDevice = properties._logicalDevice,
        ._commandPool = properties._commandPool,
        ._graphicsQueue = properties._graphicsQueue,
        ._image = _textureImage[texturePath],
        ._format = VK_FORMAT_R8G8B8A8_SRGB,
        ._oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        ._newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        ._mipLevels = _mipLevels
    };
    Utils::transitionImageLayout(transitionProperties);

    Utils::CopyBufferToImageProperties copyProperties = {
        ._logicalDevice = properties._logicalDevice,
        ._commandPool = properties._commandPool,
        ._graphicsQueue = properties._graphicsQueue,
        ._buffer = stagingBuffer,
        ._image = _textureImage[texturePath],
        ._width = (uint32_t)texWidth,
        ._height = (uint32_t)texHeight
    };
    Utils::copyBufferToImage(copyProperties);

    Utils::GenerateMipmapsProperties propertiesMipmap = {
        ._physicalDevice = properties._physicalDevice,
        ._logicalDevice = properties._logicalDevice,
        ._commandPool = properties._commandPool,
        ._graphicsQueue = properties._graphicsQueue,
        ._image = _textureImage[texturePath],
        ._mipLevels = _mipLevels,
        ._texWidth = (uint32_t)texWidth,
        ._texHeight = (uint32_t)texHeight,
        ._imageFormat = VK_FORMAT_R8G8B8A8_SRGB
    };
    Utils::generateMipmaps(propertiesMipmap);

    vkDestroyBuffer(properties._logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(properties._logicalDevice, stagingBufferMemory, nullptr);
}

/**
 * @brief Creates an image view for the swapchain's texture image.
 *
 * This function initializes the `_textureImageView` member by creating an image view
 * for the swapchain's texture image (`_textureImage`) using the specified logical device.
 * The image view is created with the `VK_FORMAT_R8G8B8A8_SRGB` format and the color aspect flag.
 *
 * @param logicalDevice The Vulkan logical device used to create the image view.
 */
void maverik::vk::SwapchainContext::createTextureImageView(VkDevice logicalDevice)
{
    for (const auto& [textureName, textureImage] : _textureImage) {
        _textureImageView[textureName] = this->createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, logicalDevice);
    }
}

/**
 * @brief Creates a Vulkan texture sampler for the swapchain context.
 *
 * This function initializes a VkSampler object with linear filtering, repeat addressing mode,
 * and enables anisotropic filtering using the maximum supported anisotropy level of the physical device.
 * The sampler is configured for use with normalized coordinates and supports mipmapping.
 *
 * @param logicalDevice The Vulkan logical device used to create the sampler.
 * @param physicalDevice The Vulkan physical device used to query device properties.
 * @param textureName The name of the texture for which the sampler is being created.
 *
 * @throws std::runtime_error If the sampler creation fails.
 */
void maverik::vk::SwapchainContext::createTextureSampler(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, const std::string& textureName, VkSamplerCreateInfo samplerInfo)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    // Check if samplerInfo is "empty" by testing its sType field.
    // If sType is not set, it's likely uninitialized.
    if (samplerInfo.sType == 0) {
        samplerInfo = this->getDefaultSamplerInfo(properties);
    }

    if (vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &_textureSampler[textureName]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler for " + textureName + " !");
    }
}

/////////////////////
// Private methods //
/////////////////////

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
VkSurfaceFormatKHR maverik::vk::SwapchainContext::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

/**
 * @brief Chooses the most suitable Vulkan present mode for the swapchain.
 *
 * This function iterates through the list of available present modes and selects
 * the most optimal one based on predefined preferences. The preferred present mode
 * is `VK_PRESENT_MODE_MAILBOX_KHR` due to its low latency and ability to avoid tearing.
 * If the preferred mode is not available, it falls back to `VK_PRESENT_MODE_FIFO_KHR`,
 * which is guaranteed to be supported and ensures vertical synchronization.
 *
 * @param availablePresentModes A vector containing the list of present modes supported
 *                              by the Vulkan surface.
 * @return VkPresentModeKHR The chosen present mode. Returns `VK_PRESENT_MODE_MAILBOX_KHR`
 *                          if available, otherwise defaults to `VK_PRESENT_MODE_FIFO_KHR`.
 */
VkPresentModeKHR maverik::vk::SwapchainContext::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/**
 * @brief Chooses the appropriate swap extent for the Vulkan swapchain.
 *
 * This function determines the dimensions of the swapchain images based on the
 * surface capabilities and the size of the window's framebuffer. If the current
 * extent of the surface capabilities is not set to the special value
 * `std::numeric_limits<uint32_t>::max()`, it directly returns the current extent.
 * Otherwise, it queries the framebuffer size of the given GLFW window and clamps
 * the dimensions to fit within the minimum and maximum image extents specified
 * by the surface capabilities.
 *
 * @param capabilities The surface capabilities that describe the supported
 *                      properties of the Vulkan surface.
 * @param window        A pointer to the GLFW window associated with the Vulkan
 *                      surface.
 * @return VkExtent2D   The chosen swap extent, which specifies the width and
 *                      height of the swapchain images.
 */
VkExtent2D maverik::vk::SwapchainContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window)
{
    int width = 0;
    int height = 0;

    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

/**
 * @brief Creates color resources for the swapchain, including a color image and its associated image view.
 *
 * This function initializes a color image with the specified format, extent, and sample count, and allocates
 * memory for it. Additionally, it creates an image view for the color image to be used in rendering operations.
 *
 * @param logicalDevice The Vulkan logical device used to create the image and image view.
 * @param physicalDevice The Vulkan physical device used to allocate memory for the image.
 * @param msaaSamples The number of samples per pixel for multisampling (MSAA).
 */
void maverik::vk::SwapchainContext::createColorResources(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits msaaSamples)
{
    VkFormat colorFormat = _swapchainFormat;
    Utils::CreateImageProperties imageProperties = {
        ._logicalDevice = logicalDevice,
        ._physicalDevice = physicalDevice,
        ._width = _swapchainExtent.width,
        ._height = _swapchainExtent.height,
        ._mipLevels = 1, // No mipmaps for color attachment
        ._format = colorFormat,
        ._tiling = VK_IMAGE_TILING_OPTIMAL,
        ._usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        ._image = _colorImage,
        ._imageMemory = _colorImageMemory
    };

    Utils::createImage(imageProperties);
    _colorImageView = this->createImageView(_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, logicalDevice);
}

/**
 * @brief Creates the depth resources required for the Vulkan swapchain.
 *
 * This function initializes the depth image, allocates memory for it, creates
 * an image view for the depth image, and transitions the image layout to be
 * suitable for use as a depth-stencil attachment.
 *
 * @param logicalDevice The Vulkan logical device used for resource creation.
 * @param physicalDevice The Vulkan physical device used to query depth format and memory properties.
 * @param commandPool The command pool used to allocate command buffers for image layout transitions.
 * @param graphicsQueue The graphics queue used to execute the image layout transition commands.
 * @param msaaSamples The number of samples per pixel for multisampling (MSAA).
 */
void maverik::vk::SwapchainContext::createDepthResources(const TextureImageCreationProperties& properties)
{
    VkFormat depthFormat = Utils::findDepthFormat(properties._physicalDevice);
    Utils::CreateImageProperties depthImageProperties = {
        ._logicalDevice = properties._logicalDevice,
        ._physicalDevice = properties._physicalDevice,
        ._width = _swapchainExtent.width,
        ._height = _swapchainExtent.height,
        ._mipLevels = 1, // No mipmaps for depth attachment
        ._format = depthFormat,
        ._tiling = VK_IMAGE_TILING_OPTIMAL,
        ._usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        ._image = _depthImage,
        ._imageMemory = _depthImageMemory
    };

    Utils::createImage(depthImageProperties);
    _depthImageView = this->createImageView(_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, properties._logicalDevice);

    Utils::TransitionImageLayoutProperties transitionProperties = {
        ._logicalDevice = properties._logicalDevice,
        ._commandPool = properties._commandPool,
        ._graphicsQueue = properties._graphicsQueue,
        ._image = _depthImage,
        ._format = depthFormat,
        ._oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        ._newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        ._mipLevels = 1 // No mipmaps for depth attachment
    };
    Utils::transitionImageLayout(transitionProperties);
}

VkSamplerCreateInfo maverik::vk::SwapchainContext::getDefaultSamplerInfo(const VkPhysicalDeviceProperties& properties)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(_mipLevels);
    return samplerInfo;
}
