/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#include "xr/SwapchainContext.hpp"

maverik::xr::SwapchainContext::SwapchainContext(XrInstance instance, XrSession session)
    : _instance(instance), _session(session)
{
}

maverik::xr::SwapchainContext::~SwapchainContext()
{
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

}
