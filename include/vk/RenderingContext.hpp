/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#ifndef RENDERINGCONTEXT_HPP_
    #define RENDERINGCONTEXT_HPP_

    #include "ARenderingContext.hpp"

    #include "vk/Utils.hpp"

    #include <map>

    /*
     * @brief Maximum number of frames in flight.
     *
     * This constant defines the maximum number of frames that can be in flight
     * at any given time. It is used to manage synchronization and resource
     * allocation for rendering operations in Vulkan.
     *
    */
    const int MAX_FRAMES_IN_FLIGHT = 2;

    /*
     * @brief Maximum number of frames in flight.
     *
     * This constant defines the maximum number of frames that can be in flight
     * at any given time. It is used to manage synchronization and resource
     * allocation for rendering operations in Vulkan.
     *
    */
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,    // Swap chain extension
        "VK_KHR_portability_subset"         // Portability subset extension
    };

    /*
     * @brief Validation layers for Vulkan debugging.
     *
     * This constant defines the validation layers used for debugging Vulkan
     * applications. These layers provide additional checks and validation
     * during development to help identify issues and improve code quality.
     *
    */
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"   // Khronos validation layer
    };

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

namespace maverik {
    namespace vk {
        class RenderingContext : public ARenderingContext {
            public:
                // Structs

                /**
                 * @brief Window properties structure.
                 *
                 * This structure holds the properties of the window, including its width,
                 * height, and title. It is used to initialize the window and set its
                 * properties during the creation of the rendering context.
                 *
                 */
                struct WindowProperties {
                    unsigned int width;     // Width of the window
                    unsigned int height;    // Height of the window
                    std::string title;      // Title of the window
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

                /**
                 * @brief Vertex structure.
                 *
                 * This structure represents a vertex in the 3D space. It contains the
                 * position, color, and texture coordinates of the vertex. It is used to
                 * define the vertex data for rendering.
                 *
                 */
                struct Vertex {
                    glm::vec3 pos;          // Position of the vertex
                    glm::vec3 color;        // Color of the vertex
                    glm::vec2 texCoord;     // Texture coordinates of the vertex

                    /*
                     * @brief Get the binding description for the vertex.
                     *
                     * This function returns the binding description for the vertex,
                     * which specifies how the vertex data is laid out in memory.
                     *
                     * @return VkVertexInputBindingDescription The binding description
                     *         for the vertex.
                     *
                    */
                    static VkVertexInputBindingDescription getBindingDescription() {
                        VkVertexInputBindingDescription bindingDescription{};
                        bindingDescription.binding = 0;
                        bindingDescription.stride = sizeof(Vertex);
                        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                        return bindingDescription;
                    }

                    /*
                     * @brief Get the attribute descriptions for the vertex.
                     *
                     * This function returns an array of attribute descriptions for the
                     * vertex, which specify the format and offset of each attribute in
                     * the vertex data.
                     *
                     * @return std::array<VkVertexInputAttributeDescription, 3> The
                     *         attribute descriptions for the vertex.
                     *
                    */
                    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
                        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

                        attributeDescriptions[0].binding = 0;
                        attributeDescriptions[0].location = 0;
                        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                        attributeDescriptions[0].offset = offsetof(Vertex, pos);

                        attributeDescriptions[1].binding = 0;
                        attributeDescriptions[1].location = 1;
                        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                        attributeDescriptions[1].offset = offsetof(Vertex, color);

                        attributeDescriptions[2].binding = 0;
                        attributeDescriptions[2].location = 2;
                        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

                        return attributeDescriptions;
                    }

                    /*
                     * @brief Equality operator for the Vertex structure.
                     *
                     * This operator compares two Vertex objects for equality based on
                     * their position, color, and texture coordinates.
                     *
                     * @param other The other Vertex object to compare with.
                     * @return true if the two Vertex objects are equal, false otherwise.
                     *
                    */
                    bool operator==(const Vertex& other) const {
                        return pos == other.pos && color == other.color && texCoord == other.texCoord;
                    }
                };

                /**
                 * @struct RenderingContextProperties
                 * @brief Holds Vulkan rendering context properties required for rendering operations.
                 *
                 * This structure encapsulates the essential Vulkan objects needed to set up and manage
                 * a rendering context, including the Vulkan instance, render pass, texture image view,
                 * and texture sampler.
                 *
                 */
                struct RenderingContextProperties {
                    /*
                     * @brief Vulkan instance associated with the rendering context.
                     *
                     * This member holds the Vulkan instance that is created and used for
                     * rendering operations. It is essential for initializing Vulkan and
                     * creating other Vulkan objects.
                     *
                    */
                    VkInstance _instance;
                    /*
                     * @brief Vulkan render pass used for rendering operations.
                     *
                     * This member holds the Vulkan render pass that defines the
                     * framebuffer attachments and their formats. It is used to manage
                     * the rendering process and how the images are presented.
                     *
                    */
                    VkRenderPass _renderPass;

                    /**
                     * @brief Associates Vulkan image views with their corresponding samplers.
                     *
                     * This map stores pairs of VkImageView and VkSampler, allowing efficient lookup
                     * of the sampler used for a particular image view. It is typically used in rendering
                     * contexts to manage texture resources and their sampling configurations.
                     */
                    std::map<VkImageView, VkSampler> _textureImageViewsAndSamplers;
                };

                // Constructors
                RenderingContext(const WindowProperties &windowProperties, const RenderingContextProperties &renderingContextProperties);

                // Destructor
                ~RenderingContext();

                void init() override {};

            protected:
                GLFWwindow *_window;                    // Pointer to the GLFW window
                VkSurfaceKHR _surface;                  // Vulkan surface for rendering
                VkQueue _presentQueue;                  // Vulkan queue for presentation

                VkPipelineLayout _pipelineLayout;       // Vulkan pipeline layout
                VkPipeline _graphicsPipeline;           // Vulkan graphics pipeline

                void createGraphicsPipeline(VkRenderPass renderPass);

                std::vector<Vertex> _vertices;          // Vector of vertices for rendering
                std::vector<uint32_t> _indices;         // Vector of indices for rendering

                void createVertexBuffer();

                VkBuffer _vertexBuffer;                                 // Vulkan buffer for vertex data
                VkDeviceMemory _vertexBufferMemory;                     // Vulkan memory for vertex buffer

                VkBuffer _indexBuffer;                                  // Vulkan buffer for index data
                VkDeviceMemory _indexBufferMemory;                      // Vulkan memory for index buffer

                std::vector<VkBuffer> _uniformBuffers;                  // Vector of Vulkan buffers for uniform data
                std::vector<VkDeviceMemory> _uniformBuffersMemory;      // Vector of Vulkan memory for uniform buffers
                std::vector<void*> _uniformBuffersMapped;               // Vector of mapped pointers to uniform buffer data

                void createIndexBuffer();
                void createUniformBuffers();

                VkDescriptorPool _descriptorPool;                       // Vulkan descriptor pool for managing descriptor sets
                std::vector<VkDescriptorSet> _descriptorSets;           // Vector of Vulkan descriptor sets for uniform data
                VkDescriptorSetLayout _descriptorSetLayout;             // Vulkan descriptor set layout for uniform data
                VkDebugUtilsMessengerEXT _debugMessenger;               // Vulkan debug messenger for validation layers

                void setupDebugMessenger(VkInstance instance);
                void createDescriptorSetLayout();
                void createDescriptorPool();
                void createDescriptorSets(std::map<VkImageView, VkSampler> imagesViewsAndSamplers);

                std::vector<VkCommandBuffer> _commandBuffers;           // Vector of Vulkan command buffers for rendering

                void createCommandBuffers();

                std::vector<VkSemaphore> _imageAvailableSemaphores;     // Vector of Vulkan semaphores for image availability
                std::vector<VkSemaphore> _renderFinishedSemaphores;     // Vector of Vulkan semaphores for rendering completion
                std::vector<VkFence> _inFlightFences;                   // Vector of Vulkan fences for synchronization

                void createSyncObjects();

                void initWindow(unsigned int width, unsigned int height, const std::string &title);
                void createSurface(VkInstance instance);
                void pickPhysicalDevice(VkInstance instance);
                void createLogicalDevice();
                void createCommandPool();

            private:
                VkSampleCountFlagBits getMaxUsableSampleCount();
                void createSingleDescriptorSets(VkImageView textureImageView, VkSampler textureSampler);

        };
    }
}

#endif /* !RENDERINGCONTEXT_HPP_ */
