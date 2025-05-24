/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#ifndef SWAPCHAINCONTEXT_HPP_
    #define SWAPCHAINCONTEXT_HPP_

    #include "ASwapchainContext.hpp"

    #include "vk/Utils.hpp"

namespace maverik {
    namespace vk {
        class SwapchainContext : public ASwapchainContext {
            public:
                struct SwapchainContextCreationProperties {
                    VkSurfaceKHR _surface;
                    VkPhysicalDevice _physicalDevice;
                    VkDevice _logicalDevice;
                    GLFWwindow *_window;
                    VkSampleCountFlagBits _msaaSamples;
                    VkCommandPool _commandPool;
                    VkQueue _graphicsQueue;
                };

                struct TextureImageCreationProperties {
                    VkPhysicalDevice _physicalDevice;
                    VkDevice _logicalDevice;
                    VkCommandPool _commandPool;
                    VkSampleCountFlagBits _msaaSamples;
                    VkQueue _graphicsQueue;
                };

                // Contructors
                SwapchainContext(const SwapchainContextCreationProperties& properties);

                // Destructor
                ~SwapchainContext();

                void recreate(const SwapchainContextCreationProperties& properties);

                void createTextureImage(const std::string& texturePath, const TextureImageCreationProperties& properties);

            protected:
                // In addition to the base swapchain
                std::vector<VkImage> _swapchainImages;

                void createImageViews(VkDevice logicalDevice);

                // Texture images
                VkImage _textureImage;
                VkDeviceMemory _textureImageMemory;
                VkImageView _textureImageView;
                VkSampler _textureSampler;

                void createTextureImageView(VkDevice logicalDevice);
                void createTextureSampler(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);

                // Depth images
                VkImage _depthImage;
                VkDeviceMemory _depthImageMemory;
                VkImageView _depthImageView;

                // Color images
                VkImage _colorImage;
                VkDeviceMemory _colorImageMemory;
                VkImageView _colorImageView;

                // Render pass
                void createRenderPass(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSampleCountFlagBits msaaSamples);

                // Used to create the swapchain
                void init(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *window);

                void cleanup(VkDevice logicalDevice);

                void createFramebuffers(VkDevice logicalDevice);

            private:
                VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
                VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
                VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window);

                void createColorResources(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits msaaSamples);
                void createDepthResources(const TextureImageCreationProperties& properties);

                VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice logicalDevice);

        };
    }
}

#endif /* !SWAPCHAINCONTEXT_HPP_ */
