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
                /**
                 * @struct SwapchainContextCreationProperties
                 * @brief Holds the properties required to create a Vulkan swapchain context.
                 *
                 * This structure encapsulates all the necessary Vulkan and GLFW objects
                 * needed for initializing and managing a swapchain context, including
                 * surface, devices, window, MSAA settings, command pool, and graphics queue.
                 *
                 */
                struct SwapchainContextCreationProperties {
                    /*
                     * @brief The Vulkan surface to present images to.
                    */
                    VkSurfaceKHR _surface;
                    /*
                     * @brief The Vulkan physical device (GPU) to use.
                    */
                    VkPhysicalDevice _physicalDevice;
                    /*
                     * @brief The Vulkan logical device associated with the physical device.
                    */
                    VkDevice _logicalDevice;
                    /*
                     * @brief A pointer to the GLFW window associated with the swapchain.
                    */
                    GLFWwindow *_window;
                    /*
                     * @brief The number of samples for multisample anti-aliasing (MSAA).
                     */
                    VkSampleCountFlagBits _msaaSamples;
                    /*
                     * @brief The Vulkan command pool used for command buffer allocation.
                     */
                    VkCommandPool _commandPool;
                    /*
                     * @brief The Vulkan graphics queue used for rendering commands.
                     */
                    VkQueue _graphicsQueue;
                };

                /**
                 * @struct TextureImageCreationProperties
                 * @brief Holds Vulkan objects and properties required for texture image creation.
                 *
                 * This structure encapsulates the necessary Vulkan handles and settings
                 * needed to create and manage texture images, including the physical and logical
                 * devices, command pool, MSAA sample count, and graphics queue.
                 *
                 */
                struct TextureImageCreationProperties {
                    /*
                     * @brief The Vulkan physical device used for resource creation.
                    */
                    VkPhysicalDevice _physicalDevice;
                    /*
                     * @brief The Vulkan logical device used for operations.
                    */
                    VkDevice _logicalDevice;
                    /*
                     * @brief The Vulkan command pool used for command buffer allocation.
                    */
                    VkCommandPool _commandPool;
                    /*
                     * @brief The number of samples for multisample anti-aliasing (MSAA).
                     */
                    VkSampleCountFlagBits _msaaSamples;
                    /*
                     * @brief The Vulkan graphics queue used for rendering operations.
                     */
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
