/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ARenderingContext
*/

#pragma once

#include <vulkan.hpp>
#include <memory>

#include "Utils.hpp"

/**
 * @struct VulkanContext
 * @brief Represents the Vulkan rendering context, encapsulating essential Vulkan objects and configurations.
 *
 * This structure is used to manage and store Vulkan-specific resources required for rendering operations.
 *
 * @var VulkanContext::logicalDevice
 * The Vulkan logical device used for rendering operations.
 *
 * @var VulkanContext::physicalDevice
 * The Vulkan physical device representing the GPU hardware.
 *
 * @var VulkanContext::graphicsQueue
 * The Vulkan queue used for submitting graphics commands.
 *
 * @var VulkanContext::renderPass
 * The Vulkan render pass object defining the rendering pipeline.
 *
 * @var VulkanContext::commandPool
 * The Vulkan command pool used for allocating command buffers.
 *
 * @var VulkanContext::graphicsQueueFamilyIndex
 * The index of the queue family that supports graphics operations.
 */
struct  VulkanContext{
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    uint32_t graphicsQueueFamilyIndex;
    VkSurfaceKHR surface;
    GLFWwindow* window;
    VkSampleCountFlagBits msaaSamples;
};

namespace maverik {
    class ARenderingContext {
        public:
            virtual ~ARenderingContext() = default;

            virtual void init() = 0;

            /**
             * @brief Retrieves the Vulkan context associated with this rendering context.
             *
             * @return A constant reference to a shared pointer of VulkanContext.
             */
            const std::shared_ptr<VulkanContext>& getVulkanContext() const {
                return _vulkanContext;
            }

            /**
             * @brief Creates a Vulkan render pass for the rendering context.
             *
             * This function sets up a render pass with color, depth, and resolve attachments,
             * supporting multisample anti-aliasing (MSAA) as specified by the msaaSamples parameter.
             * The render pass is configured for use in a graphics pipeline, with appropriate
             * subpass and dependency settings for color and depth outputs.
             *
             * @param physicalDevice The Vulkan physical device used to determine supported formats.
             * @param logicalDevice The Vulkan logical device used to create the render pass.
             * @param msaaSamples The number of samples per pixel for MSAA (multisample anti-aliasing).
             *
             * @throws std::runtime_error If the render pass creation fails.
             */
            void createRenderPass(VkSurfaceKHR surface)
            {
                auto swapchainSupport = maverik::Utils::querySwapChainSupport(_physicalDevice, surface);
                auto swapchainFormat = this->chooseSwapSurfaceFormat(swapchainSupport.formats);

                VkAttachmentDescription colorAttachment{};
                colorAttachment.format = swapchainFormat.format;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.samples = _msaaSamples;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentDescription depthAttachment{};
                depthAttachment.format = maverik::Utils::findDepthFormat(_physicalDevice);
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depthAttachment.samples = _msaaSamples;

                VkAttachmentReference colorAttachmentRef{};
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthAttachmentRef{};
                depthAttachmentRef.attachment = 1;
                depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkAttachmentDescription colorAttachmentResolve{};
                colorAttachmentResolve.format = swapchainFormat.format;
                colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentReference colorAttachmentResolveRef{};
                colorAttachmentResolveRef.attachment = 2;
                colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass{};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorAttachmentRef;
                subpass.pDepthStencilAttachment = &depthAttachmentRef;
                subpass.pResolveAttachments = &colorAttachmentResolveRef;

                VkSubpassDependency dependency{};
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.srcAccessMask = 0;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                std::array<VkAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
                VkRenderPassCreateInfo renderPassInfo{};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                renderPassInfo.pAttachments = attachments.data();
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpass;
                renderPassInfo.dependencyCount = 1;
                renderPassInfo.pDependencies = &dependency;

                if (vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create render pass!");
                }
            }

        private:
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
            VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
            {
                for (const auto& availableFormat : availableFormats) {
                    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        return availableFormat;
                    }
                }
                return availableFormats[0];
            }

        protected:

            virtual void pickPhysicalDevice(VkInstance instance) = 0;

            virtual void createLogicalDevice() = 0;

            virtual void createCommandPool() = 0;

            VkSampleCountFlagBits getMaxUsableSampleCount() const;

            VkDevice _logicalDevice;
            VkPhysicalDevice _physicalDevice;
            VkQueue _graphicsQueue;
            VkCommandPool _commandPool;
            VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

            std::shared_ptr<VulkanContext> _vulkanContext;

            VkRenderPass _renderPass;
    };
}
