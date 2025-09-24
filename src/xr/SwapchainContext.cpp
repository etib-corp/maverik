/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#include "xr/SwapchainContext.hpp"

maverik::xr::SwapchainContext::SwapchainContext(const SwapchainContextCreationPropertiesXR& properties) :
    _instance(properties._instance),
    _systemId(properties._systemId),
    _session(properties._session),
    _physicalDevice(properties._physicalDevice),
    _device(properties._device)
{
    init();
}

maverik::xr::SwapchainContext::~SwapchainContext()
{
}

uint64_t maverik::xr::SwapchainContext::selectSwapchainFormat(const std::vector<int64_t> &swapchainFormats)
{
    constexpr VkFormat kPreferredSwapchainFormats[] = {
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM
    };

    auto swapchainFormatIt = std::find_first_of(swapchainFormats.begin(), swapchainFormats.end(),
                        std::begin(kPreferredSwapchainFormats),
                        std::end(kPreferredSwapchainFormats));

    if (swapchainFormatIt == swapchainFormats.end()) {
        std::cerr << "SwapchainContext: No preferred swapchain format found, using first available format" << std::endl;
        return swapchainFormats.empty() ? 0 : swapchainFormats[0];
    }
    return *swapchainFormatIt;
}

void maverik::xr::SwapchainContext::init()
{
    if (_instance == XR_NULL_HANDLE || _session == XR_NULL_HANDLE) {
        std::cerr << "SwapchainContext: Invalid instance or session" << std::endl;
        return;
    }

    uint32_t swapchainFormatCount = 0;
    xrEnumerateSwapchainFormats(_session, 0, &swapchainFormatCount, nullptr);
    if (swapchainFormatCount == 0) {
        std::cerr << "SwapchainContext: No swapchain formats available" << std::endl;
        return;
    }
    std::vector<int64_t> swapchainFormats(swapchainFormatCount);
    xrEnumerateSwapchainFormats(_session, swapchainFormatCount, &swapchainFormatCount, swapchainFormats.data());

    uint32_t viewCount = 0;
    xrEnumerateViewConfigurationViews(_instance, _systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr);
    if (viewCount == 0) {
        std::cerr << "SwapchainContext: No view configurations available" << std::endl;
        return;
    }
    _viewsConfigurations.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    xrEnumerateViewConfigurationViews(_instance, _systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, viewCount, &viewCount, _viewsConfigurations.data());
    _views.resize(viewCount, {XR_TYPE_VIEW});

    _swapchainColorFormat = selectSwapchainFormat(swapchainFormats);

    for (const auto &viewConfig : _viewsConfigurations) {
        XrSwapchainCreateInfo swapchainCreateInfo = {};
        swapchainCreateInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
        swapchainCreateInfo.arraySize = 1;
        swapchainCreateInfo.format = _swapchainColorFormat;
        swapchainCreateInfo.width = viewConfig.recommendedImageRectWidth;
        swapchainCreateInfo.height = viewConfig.recommendedImageRectHeight;
        swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
        ASwapchain<XrSwapchain> swapchain;
        swapchain._height = viewConfig.recommendedImageRectHeight;
        swapchain._width = viewConfig.recommendedImageRectWidth;

        XrResult result = xrCreateSwapchain(_session, &swapchainCreateInfo, &swapchain.swapchain);
        if (result != XR_SUCCESS) {
            std::cerr << "SwapchainContext: Failed to create swapchain, error: " << result << std::endl;
            return;
        }
        _swapchain.push_back(swapchain);
        std::shared_ptr<maverik::xr::SwapChainImage> swapchainImage = createSwapchainImage(swapchainCreateInfo, swapchain);

        _swapchainImages[swapchain.swapchain] = swapchainImage;

        XrSwapchainImageBaseHeader *image = swapchainImage->getFirstImagePointer();

        xrEnumerateSwapchainImages(swapchain.swapchain, swapchainImage->_swapchainImages.size(), nullptr, image);
    }
}

std::shared_ptr<maverik::xr::SwapChainImage> maverik::xr::SwapchainContext::createSwapchainImage(XrSwapchainCreateInfo &swapchainCreateInfo, ASwapchain<XrSwapchain> &swapchain)
{
    uint32_t imageCount = 0;
    xrEnumerateSwapchainImages(swapchain.swapchain, 0, &imageCount, nullptr);

    SwapchainImageCreationPropertiesXR properties;
    properties._capacity = imageCount;
    properties._device = _device;
    properties._physicalDevice = _physicalDevice;
    properties._swapchainCreateInfo = swapchainCreateInfo;

    std::shared_ptr<maverik::xr::SwapChainImage> swapchainImage = std::make_shared<SwapChainImage>();
    swapchainImage->init(properties);

    return swapchainImage;
}


void maverik::xr::SwapChainImage::init(const SwapchainImageCreationPropertiesXR &properties)
{
    _device = properties._device;
    _physicalDevice = properties._physicalDevice;

    _swapchainImageFormat = static_cast<VkFormat>(properties._swapchainCreateInfo.format);
    _swapchainExtent = {properties._swapchainCreateInfo.width, properties._swapchainCreateInfo.height};

    _swapchainImages.resize(properties._capacity);
    _swapchainImageViews.resize(properties._capacity);
    _swapchainFrameBuffers.resize(properties._capacity);

    _viewport = {
        .x = 0,
        .y = static_cast<float>(_swapchainExtent.height),
        .width = static_cast<float>(_swapchainExtent.width),
        .height = -static_cast<float>(_swapchainExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    _scissor.extent = {
        .width = _swapchainExtent.width,
        .height = _swapchainExtent.height
    };

    for (auto &image : _swapchainImages) {
        image.type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR;
    }

}

void maverik::xr::SwapChainImage::createColorResources()
{
    Utils::CreateImageProperties properties = {
        ._logicalDevice = _device,
        ._physicalDevice = _physicalDevice,
        ._width = _swapchainExtent.width,
        ._height = _swapchainExtent.height,
        ._mipLevels = 1,
        ._format = _swapchainImageFormat,
        ._tiling = VK_IMAGE_TILING_OPTIMAL,
        ._usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        ._image = _colorImage,
        ._imageMemory = _depthImageMemory,
    };
    Utils::createImage(properties);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _colorImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = _swapchainImageFormat;
    viewInfo.subresourceRange = {
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1
    };

    if (vkCreateImageView(_device, &viewInfo, nullptr, &_colorImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
    }

    Utils::TransitionImageLayoutProperties transitionProperties = {
        ._logicalDevice = _device,
        ._commandPool = VK_NULL_HANDLE, // Set your command pool here
        ._graphicsQueue = VK_NULL_HANDLE, // Set your graphics queue here
        ._image = _colorImage,
        ._format = _swapchainImageFormat,
        ._oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        ._newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        ._mipLevels = 1
    };
    Utils::transitionImageLayout(transitionProperties);
}

void maverik::xr::SwapChainImage::createDepthResources()
{
    VkFormat depthFormat = Utils::findSupportedDepthFormat(_physicalDevice);
    Utils::CreateImageProperties properties = {
        ._logicalDevice = _device,
        ._physicalDevice = _physicalDevice,
        ._width = _swapchainExtent.width,
        ._height = _swapchainExtent.height,
        ._mipLevels = 1,
        ._format = depthFormat,
        ._tiling = VK_IMAGE_TILING_OPTIMAL,
        ._usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        ._properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        ._image = _depthImage,
        ._imageMemory = _depthImageMemory
    };
    Utils::createImage(properties);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = _depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange = {
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1
    };
    if (vkCreateImageView(_device, &viewInfo, nullptr, &_depthImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
    }

    Utils::TransitionImageLayoutProperties transitionProperties = {
        ._logicalDevice = _device,
        ._commandPool = VK_NULL_HANDLE, // Set your command pool here
        ._graphicsQueue = VK_NULL_HANDLE, // Set your graphics queue here
        ._image = _depthImage,
        ._format = depthFormat,
        ._oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        ._newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        ._mipLevels = 1
    };
    Utils::transitionImageLayout(transitionProperties);

}

void maverik::xr::SwapChainImage::createFrameBuffers()
{
    for (size_t i = 0; i < _swapchainImageViews.size(); i++) {
        VkImageView attachments[] = {
            _colorImageView,
            _depthImageView,
            _swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = nullptr; // Set your render pass here
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _swapchainExtent.width;
        framebufferInfo.height = _swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapchainFrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }

    Utils::TransitionImageLayoutProperties transitionProperties = {
        ._logicalDevice = _device,
        ._commandPool = VK_NULL_HANDLE, // Set your command pool here
        ._graphicsQueue = VK_NULL_HANDLE, // Set your graphics queue here
        ._image = _colorImage,
        ._format = _swapchainImageFormat,
        ._oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        ._newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        ._mipLevels = 1
    };
    Utils::transitionImageLayout(transitionProperties);
}
