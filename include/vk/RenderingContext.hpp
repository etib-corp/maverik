/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#ifndef RENDERINGCONTEXT_HPP_
    #define RENDERINGCONTEXT_HPP_

    #include "ARenderingContext.hpp"

    #include "Vertex.hpp"

    #include "Utils.hpp"

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

                // Constructors
                RenderingContext(const WindowProperties &windowProperties, VkInstance instance);

                // Destructor
                ~RenderingContext();

                void init() override {};

            protected:
                GLFWwindow *_window;                    // Pointer to the GLFW window
                VkSurfaceKHR _surface;                  // Vulkan surface for rendering
                VkQueue _presentQueue;                  // Vulkan queue for presentation

                std::vector<Vertex> _vertices;          // Vector of vertices for rendering
                std::vector<uint32_t> _indices;         // Vector of indices for rendering

                void createVertexBuffer();

                VkBuffer _vertexBuffer;                                 // Vulkan buffer for vertex data
                VkDeviceMemory _vertexBufferMemory;                     // Vulkan memory for vertex buffer

                VkBuffer _indexBuffer;                                  // Vulkan buffer for index data
                VkDeviceMemory _indexBufferMemory;                      // Vulkan memory for index buffer

                void createIndexBuffer();

                std::vector<VkCommandBuffer> _commandBuffers;           // Vector of Vulkan command buffers for rendering

                void createCommandBuffers();

                std::vector<VkSemaphore> _imageAvailableSemaphores;     // Vector of Vulkan semaphores for image availability
                std::vector<VkSemaphore> _renderFinishedSemaphores;     // Vector of Vulkan semaphores for rendering completion
                std::vector<VkFence> _inFlightFences;                   // Vector of Vulkan fences for synchronization

                void createSyncObjects();

                void initWindow(unsigned int width, unsigned int height, const std::string &title);
                void createSurface(VkInstance instance);
                void pickPhysicalDevice(VkInstance instance) override;
                void createLogicalDevice() override;
                void createCommandPool() override;

            private:
                VkSampleCountFlagBits getMaxUsableSampleCount();

        };
    }
}

#endif /* !RENDERINGCONTEXT_HPP_ */
