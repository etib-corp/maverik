/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#ifndef SWAPCHAINCONTEXT_HPP_
    #define SWAPCHAINCONTEXT_HPP_

    #include "ASwapchainContext.hpp"

    #include "Vertex.hpp"

    #include "Utils.hpp"

    #include <map>

    #ifndef MAX_FRAMES_IN_FLIGHT
        #define MAX_FRAMES_IN_FLIGHT 2
    #endif

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
                    /*
                     * @brief The Vulkan render pass used for rendering operations.
                     */
                    VkRenderPass _renderPass;
                    /*
                     * @brief The Vulkan instance associated with the swapchain context.
                     */
                    VkInstance _instance;
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

                /**
                 * @brief Uniform buffer object structure.
                 *
                 * This structure represents the data that will be passed to the vertex
                 * shader as a uniform buffer. It contains the model, view, and projection
                 * matrices used for rendering.
                 *
                 */
                struct UniformBufferObject {
                    glm::mat4 model;    // Model matrix
                    glm::mat4 view;     // View matrix
                    glm::mat4 proj;     // Projection matrix
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
                std::map<std::string, VkImage> _textureImage;
                VkDeviceMemory _textureImageMemory;
                std::map<std::string, VkImageView> _textureImageView;
                std::map<std::string, VkSampler> _textureSampler;

                void createTextureImageView(VkDevice logicalDevice);
                void createTextureSampler(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, const std::string& textureName, VkSamplerCreateInfo samplerInfo = {});

                // Depth images
                VkImage _depthImage;
                VkDeviceMemory _depthImageMemory;
                VkImageView _depthImageView;

                // Color images
                VkImage _colorImage;
                VkDeviceMemory _colorImageMemory;
                VkImageView _colorImageView;

                // Used to create the swapchain
                void init(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice logicalDevice, GLFWwindow *window);

                void cleanup(VkDevice logicalDevice);

                void createFramebuffers(VkDevice logicalDevice, VkRenderPass renderPass);

                VkPipelineLayout _pipelineLayout;       // Vulkan pipeline layout
                VkPipeline _graphicsPipeline;           // Vulkan graphics pipeline

                void createGraphicsPipeline(VkRenderPass renderPass, VkDevice logicalDevice, VkSampleCountFlagBits msaaSamples);

                VkDescriptorPool _descriptorPool;                       // Vulkan descriptor pool for managing descriptor sets
                std::vector<VkDescriptorSet> _descriptorSets;           // Vector of Vulkan descriptor sets for uniform data
                VkDescriptorSetLayout _descriptorSetLayout;             // Vulkan descriptor set layout for uniform data
                VkDebugUtilsMessengerEXT _debugMessenger;               // Vulkan debug messenger for validation layers

                void setupDebugMessenger(VkInstance instance);
                void createDescriptorSetLayout(VkDevice logicalDevice);
                void createDescriptorPool(VkDevice logicalDevice);
                void createDescriptorSets(VkDevice logicalDevice, std::map<VkImageView, VkSampler> imagesViewsAndSamplers);

                std::vector<VkBuffer> _uniformBuffers;                  // Vector of Vulkan buffers for uniform data
                std::vector<VkDeviceMemory> _uniformBuffersMemory;      // Vector of Vulkan memory for uniform buffers
                std::vector<void*> _uniformBuffersMapped;               // Vector of mapped pointers to uniform buffer data

                void createUniformBuffers(VkDevice logicalDevice, VkPhysicalDevice physicalDevice);

            private:
                VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
                VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
                VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window);

                void createColorResources(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkSampleCountFlagBits msaaSamples);
                void createDepthResources(const TextureImageCreationProperties& properties);

                VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice logicalDevice);

                VkSamplerCreateInfo getDefaultSamplerInfo(const VkPhysicalDeviceProperties& properties);

                void createSingleDescriptorSets(VkDevice logicalDevice, VkImageView textureImageView, VkSampler textureSampler);

        };
    }
}

#endif /* !SWAPCHAINCONTEXT_HPP_ */
