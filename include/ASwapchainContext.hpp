/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ASwapchainContext
*/

#pragma once

#include "ASwapchain.hpp"
#include "maverik.hpp"

#include <memory>
#include <vector>

namespace maverik {
    class ASwapchainContext {
        public:
            virtual ~ASwapchainContext() = default;

            virtual uint64_t getSwapchainFormat() const {
                return _swapchainColorFormat;
            }

        protected:

            virtual void createRenderPass() = 0;

            virtual void createGraphicsPipeline() = 0;

            std::vector<VkImageView> _imageViews;
            VkFormat _swapchainFormat;
            VkExtent2D _swapchainExtent;
            std::vector<VkFramebuffer> _swapchainFramebuffers;

            uint32_t _mipLevels;

            VkFormat _swapchainColorFormat = VK_FORMAT_UNDEFINED;

            VkRenderPass _renderPass = VK_NULL_HANDLE;

            VkPipelineLayout _pipelineLayout;       // Vulkan pipeline layout
            VkPipeline _graphicsPipeline;           // Vulkan graphics pipeline

            #if defined(XIDER_VK_IMPLEMENTATION)
                ASwapchain<VkSwapchainKHR> _swapchain;
            #elif defined(XIDER_XR_IMPLEMENTATION)
                std::vector<ASwapchain<XrSwapchain>> _swapchain;
            #endif
    };
}
