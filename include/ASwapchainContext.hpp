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

            /*
            * @brief Get the format of the swapchain images.
            *
            * @return The format of the swapchain images.
            */
            virtual uint64_t getSwapchainFormat() const {
                return _swapchainColorFormat;
            }

        protected:
            /**
             * @brief Creates and initializes the render pass for the swapchain context.
             *
             * This pure virtual function must be implemented by derived classes to set up
             * the render pass, which defines how rendering operations are performed and how
             * framebuffers are used within the swapchain context.
             *
             * @note This function should be called before starting any rendering operations
             * that depend on the render pass configuration.
             */
            virtual void createRenderPass() = 0;

            /**
             * @brief Creates and initializes the graphics pipeline for rendering operations.
             *
             * This pure virtual function must be implemented by derived classes to set up
             * the graphics pipeline, including shader stages, fixed-function state, and
             * any other necessary pipeline configurations required for rendering.
             *
             * @note This function should be called before issuing any draw commands.
             */
            virtual void createGraphicsPipeline() = 0;

            std::vector<VkImageView> _imageViews;           // Image views for the swapchain images
            VkExtent2D _swapchainExtent;                    // Dimensions of the swapchain images
            std::vector<VkFramebuffer> _swapchainFramebuffers;      // Framebuffers for the swapchain images

            uint32_t _mipLevels;                       // Number of mipmap levels for textures

            VkFormat _swapchainColorFormat = VK_FORMAT_UNDEFINED;   // Format of the swapchain images

            VkRenderPass _renderPass = VK_NULL_HANDLE;        // Vulkan render pass

            VkPipelineLayout _pipelineLayout;       // Vulkan pipeline layout
            VkPipeline _graphicsPipeline;           // Vulkan graphics pipeline

            #if defined(XIDER_VK_IMPLEMENTATION)
                ASwapchain<VkSwapchainKHR> _swapchain;  // Vulkan swapchain
            #elif defined(XIDER_XR_IMPLEMENTATION)
                std::vector<ASwapchain<XrSwapchain>> _swapchain;    // OpenXR swapchains
            #endif
    };
}
