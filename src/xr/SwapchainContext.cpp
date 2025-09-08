/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#include "xr/SwapchainContext.hpp"

maverik::xr::SwapchainContext::SwapchainContext(XrInstance instance, XrSystemId systemID, XrSession session)
    : _systemID(systemID),
      _instance(instance),
      _session(session)
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
    xrEnumerateViewConfigurationViews(_instance, _systemID, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr);
    if (viewCount == 0) {
        std::cerr << "SwapchainContext: No view configurations available" << std::endl;
        return;
    }
    _viewsConfigurations.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    xrEnumerateViewConfigurationViews(_instance, _systemID, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, viewCount, &viewCount, _viewsConfigurations.data());
    _views.resize(viewCount, {XR_TYPE_VIEW});

    _swapchaineFormat = selectSwapchainFormat(swapchainFormats);

    for (const auto &viewConfig : _viewsConfigurations) {
        XrSwapchainCreateInfo swapchainCreateInfo = {};
        swapchainCreateInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
        swapchainCreateInfo.arraySize = 1;
        swapchainCreateInfo.format = _swapchaineFormat;
        swapchainCreateInfo.width = viewConfig.recommendedImageRectWidth;
        swapchainCreateInfo.height = viewConfig.recommendedImageRectHeight;
        swapchainCreateInfo.mipCount = 1;
        swapchainCreateInfo.faceCount = 1;
        swapchainCreateInfo.sampleCount = viewConfig.recommendedSwapchainSampleCount;
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
    }
}
