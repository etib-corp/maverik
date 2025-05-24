/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ASwapchainContext
*/

#pragma once

#include "ASwapchain.hpp"
#include "vulkan.hpp"
#include <memory>
#include <vector>

namespace maverik {
    class ASwapchainContext {
        public:
            virtual ~ASwapchainContext() = default;

        protected:
            std::vector<VkImageView> _imageViews;
            VkFormat _swapchainFormat;
            VkExtent2D _swapchainExtent;
            std::vector<VkFramebuffer> _swapchainFramebuffers;

            VkRenderPass _renderPass;

            uint32_t _mipLevels;


            #if defined(XIDER_VK_IMPLEMENTATION)
                ASwapchain<VkSwapchainKHR> _swapchain;
            #elif defined(XIDER_XR_IMPLEMENTATION)
                ASwapchain<XrSwapchain> _swapchain;
            #endif
    };
}
