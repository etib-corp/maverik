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

    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_portability_subset"
    };

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
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
                struct WindowProperties {
                    unsigned int width;
                    unsigned int height;
                    std::string title;
                };

                struct UniformBufferObject {
                    glm::mat4 model;
                    glm::mat4 view;
                    glm::mat4 proj;
                };

                struct Vertex {
                    glm::vec3 pos;
                    glm::vec3 color;
                    glm::vec2 texCoord;

                    static VkVertexInputBindingDescription getBindingDescription() {
                        VkVertexInputBindingDescription bindingDescription{};
                        bindingDescription.binding = 0;
                        bindingDescription.stride = sizeof(Vertex);
                        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                        return bindingDescription;
                    }

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

                    bool operator==(const Vertex& other) const {
                        return pos == other.pos && color == other.color && texCoord == other.texCoord;
                    }
                };

                // Constructors
                RenderingContext(const WindowProperties &windowProperties, VkInstance instance, VkRenderPass renderPass, VkImageView textureImageView, VkSampler textureSampler);
                ~RenderingContext();

            protected:
                GLFWwindow *_window;
                VkSurfaceKHR _surface;
                VkQueue _presentQueue;

                VkPipelineLayout _pipelineLayout;
                VkPipeline _graphicsPipeline;

                void createGraphicsPipeline(VkRenderPass renderPass);

                std::vector<Vertex> _vertices;
                std::vector<uint32_t> _indices;

                void createVertexBuffer();

                VkBuffer _vertexBuffer;
                VkDeviceMemory _vertexBufferMemory;

                VkBuffer _indexBuffer;
                VkDeviceMemory _indexBufferMemory;

                std::vector<VkBuffer> _uniformBuffers;
                std::vector<VkDeviceMemory> _uniformBuffersMemory;
                std::vector<void*> _uniformBuffersMapped;

                void createIndexBuffer();
                void createUniformBuffers();

                VkDescriptorPool _descriptorPool;
                std::vector<VkDescriptorSet> _descriptorSets;
                VkDescriptorSetLayout _descriptorSetLayout;
                VkDebugUtilsMessengerEXT _debugMessenger;

                void setupDebugMessenger(VkInstance instance);
                void createDescriptorSetLayout();
                void createDescriptorPool();
                void createDescriptorSets(VkImageView textureImageView, VkSampler textureSampler);

                std::vector<VkCommandBuffer> _commandBuffers;

                void createCommandBuffers();

                std::vector<VkSemaphore> _imageAvailableSemaphores;
                std::vector<VkSemaphore> _renderFinishedSemaphores;
                std::vector<VkFence> _inFlightFences;

                void createSyncObjects();

                void initWindow(unsigned int width, unsigned int height, const std::string &title);
                void createSurface(VkInstance instance);
                void pickPhysicalDevice(VkInstance instance);
                void createLogicalDevice();
                void createCommandPool();

            private:
                VkSampleCountFlagBits getMaxUsableSampleCount();

        };
    }
}

#endif /* !RENDERINGCONTEXT_HPP_ */
