/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#include "vk/RenderingContext.hpp"

////////////////////
// Static methods //
////////////////////

/**
 * @brief Default debug callback function for Vulkan validation layers.
 *
 * This function is called whenever a validation layer generates a debug message.
 * It outputs the message to the standard error stream.
 *
 * @param messageSeverity Specifies the severity of the message (e.g., verbose, info, warning, or error).
 * @param messageType Specifies the type of the message (e.g., general, validation, or performance).
 * @param pCallbackData Pointer to a structure containing details about the debug message.
 * @param pUserData Pointer to user-defined data passed during the creation of the debug messenger.
 *
 * @return Always returns VK_FALSE, indicating that the Vulkan call that triggered the callback should not be aborted.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

////////////////////
// Public methods //
////////////////////

/**
 * @brief Constructs a RenderingContext object and initializes all Vulkan-related resources.
 *
 * This constructor sets up the Vulkan rendering context by initializing the window,
 * creating the Vulkan surface, selecting a physical device, creating a logical device,
 * and setting up various Vulkan resources such as descriptor sets, pipelines, buffers,
 * and synchronization objects.
 *
 * @param windowProperties The properties of the window, including width, height, and title.
 * @param instance The Vulkan instance to be used for creating the rendering context.
 * @param renderPass The Vulkan render pass to be used in the graphics pipeline.
 * @param textureImageView The Vulkan image view for the texture to be used in the descriptor sets.
 * @param textureSampler The Vulkan sampler for the texture to be used in the descriptor sets.
 */
maverik::vk::RenderingContext::RenderingContext(const WindowProperties &windowProperties, VkInstance instance)
{
    this->initWindow(windowProperties.width, windowProperties.height, windowProperties.title);
    this->createSurface(instance);
    this->pickPhysicalDevice(instance);
    this->createLogicalDevice();
    this->createCommandPool();
    this->createVertexBuffer();
    this->createIndexBuffer();
    this->createCommandBuffers();
    this->createSyncObjects();

    _vulkanContext = std::make_shared<VulkanContext>();
    _vulkanContext->logicalDevice = _logicalDevice;
    _vulkanContext->physicalDevice = _physicalDevice;
    _vulkanContext->graphicsQueue = _graphicsQueue;
    _vulkanContext->commandPool = _commandPool;
    _vulkanContext->graphicsQueueFamilyIndex = Utils::findQueueFamilies(_physicalDevice, _surface).graphicsFamily.value();
    _vulkanContext->surface = _surface;
    _vulkanContext->window = _window;
    _vulkanContext->msaaSamples = _msaaSamples;
}

/**
 * @brief Destructor for the RenderingContext class.
 *
 * Cleans up resources associated with the RenderingContext instance.
 * This destructor is invoked when an object of the RenderingContext
 * class goes out of scope or is explicitly deleted.
 */
maverik::vk::RenderingContext::~RenderingContext()
{
}

/**
 * @brief Initializes a GLFW window for the rendering context.
 *
 * This function sets up a GLFW window with the specified dimensions and title.
 * It ensures that GLFW is properly initialized and configures the window to
 * use Vulkan as the rendering API. If any step fails, an exception is thrown.
 *
 * @param width The width of the window in pixels.
 * @param height The height of the window in pixels.
 * @param title The title of the window as a string.
 *
 * @throws std::runtime_error If GLFW initialization fails or the window cannot be created.
 */
void maverik::vk::RenderingContext::initWindow(unsigned int width, unsigned int height, const std::string &title)
{
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }
}

///////////////////////
// Protected methods //
///////////////////////

/**
 * @brief Creates a Vulkan surface for rendering, specific to the platform.
 *
 * This function initializes a Vulkan surface (_surface) for the given Vulkan instance.
 * It handles platform-specific surface creation logic. On Windows, it uses the
 * `vkCreateWin32SurfaceKHR` function, while on other platforms, it uses GLFW's
 * `glfwCreateWindowSurface` function.
 *
 * @param instance The Vulkan instance used to create the surface. Must not be VK_NULL_HANDLE.
 *
 * @throws std::runtime_error If the Vulkan instance is null or if surface creation fails.
 */
void maverik::vk::RenderingContext::createSurface(VkInstance instance)
{
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(_window);
    createInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
#else
    if (instance == VK_NULL_HANDLE) {
        throw std::runtime_error("Vulkan instance is NULL!");
    }
    if (glfwCreateWindowSurface(instance, _window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
#endif
}

/**
 * @brief Selects and initializes a suitable physical device (GPU) for Vulkan operations.
 *
 * This function enumerates all available physical devices and selects the first one
 * that meets the application's requirements. It ensures that the selected device
 * supports the required Vulkan extensions and is compatible with the provided surface.
 *
 * @param instance The Vulkan instance used to enumerate physical devices.
 *
 * @throws std::runtime_error If no GPUs with Vulkan support are found or if no suitable
 *         GPU is available that meets the application's requirements.
 */
void maverik::vk::RenderingContext::pickPhysicalDevice(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support !");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (Utils::isDeviceSuitable(device, _surface, deviceExtensions)) {
            _physicalDevice = device;
            _msaaSamples = this->getMaxUsableSampleCount();
            break;
        }
    }

    if (_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

/**
 * @brief Creates a Vulkan logical device and retrieves the graphics and present queues.
 *
 * This function sets up a logical device for the Vulkan application by specifying
 * the required queue families, device features, and extensions. It also retrieves
 * the graphics and present queues for later use in rendering and presentation.
 *
 * @throws std::runtime_error if the logical device creation fails.
 *
 * The function performs the following steps:
 * - Finds the queue families required for graphics and presentation.
 * - Configures the queue creation information for the unique queue families.
 * - Specifies the required device features, such as anisotropic filtering and sample rate shading.
 * - Configures the logical device creation information, including extensions and validation layers.
 * - Creates the logical device using `vkCreateDevice`.
 * - Retrieves the graphics and present queues using `vkGetDeviceQueue`.
 */
void maverik::vk::RenderingContext::createLogicalDevice()
{
    Utils::QueueFamilyIndices indices = Utils::findQueueFamilies(_physicalDevice, _surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(_logicalDevice, indices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_logicalDevice, indices.presentFamily.value(), 0, &_presentQueue);
}

/**
 * @brief Creates a Vulkan command pool for managing command buffers.
 *
 * This function initializes a command pool that is used to allocate and manage
 * command buffers for recording Vulkan commands. The command pool is created
 * with the reset command buffer flag, allowing individual command buffers to be
 * reset independently.
 *
 * @throws std::runtime_error If the command pool creation fails.
 *
 * @details
 * - The function retrieves the queue family indices for the physical device and
 *   surface using the `Utils::findQueueFamilies` utility function.
 * - The command pool is associated with the graphics queue family index.
 * - The Vulkan function `vkCreateCommandPool` is used to create the command pool.
 *
 * @note Ensure that the `_physicalDevice`, `_surface`, and `_logicalDevice` are
 * properly initialized before calling this function.
 */
void maverik::vk::RenderingContext::createCommandPool()
{
    Utils::QueueFamilyIndices queueFamilyIndices = Utils::findQueueFamilies(_physicalDevice, _surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

/**
 * @brief Creates a vertex buffer and uploads vertex data to the GPU.
 *
 * This function creates a staging buffer in host-visible memory, copies the vertex data
 * to it, and then transfers the data to a device-local vertex buffer for optimal GPU access.
 * The staging buffer is destroyed after the data transfer is complete.
 *
 * @details
 * - A staging buffer is created with `VK_BUFFER_USAGE_TRANSFER_SRC_BIT` and
 *   `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT` properties.
 * - The vertex data is mapped to the staging buffer memory and copied using `memcpy`.
 * - A device-local vertex buffer is created with `VK_BUFFER_USAGE_TRANSFER_DST_BIT` and
 *   `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT` properties.
 * - The data is transferred from the staging buffer to the vertex buffer using a command buffer.
 * - The staging buffer and its associated memory are cleaned up after the transfer.
 *
 * @note This function assumes that `_vertices` contains the vertex data to be uploaded.
 *
 * @throws Vulkan-related errors if buffer creation, memory mapping, or data transfer fails.
 */
void maverik::vk::RenderingContext::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Utils::CreateBufferProperties stagingBufferProperties = {
        ._logicalDevice = _logicalDevice,
        ._physicalDevice = _physicalDevice,
        ._size = bufferSize,
        ._usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        ._buffer = stagingBuffer,
        ._bufferMemory = stagingBufferMemory
    };

    Utils::createBuffer(stagingBufferProperties);

    void* data;
    vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, _vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(_logicalDevice, stagingBufferMemory);

    Utils::CreateBufferProperties vertexBufferProperties = {
        ._logicalDevice = _logicalDevice,
        ._physicalDevice = _physicalDevice,
        ._size = bufferSize,
        ._usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        ._buffer = _vertexBuffer,
        ._bufferMemory = _vertexBufferMemory
    };
    Utils::createBuffer(vertexBufferProperties);

    Utils::CopyBufferProperties copyBufferProperties = {
        ._logicalDevice = _logicalDevice,
        ._commandPool = _commandPool,
        ._graphicsQueue = _graphicsQueue,
        ._srcBuffer = stagingBuffer,
        ._dstBuffer = _vertexBuffer,
        ._size = bufferSize
    };
    Utils::copyBuffer(copyBufferProperties);

    vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);
}

/**
 * @brief Creates an index buffer for the Vulkan rendering context.
 *
 * This function initializes the index buffer by creating a staging buffer,
 * copying the index data to it, and then transferring the data to a device-local
 * buffer for optimal GPU access. The staging buffer is destroyed after the data
 * transfer is complete.
 *
 * @details
 * - A staging buffer is created with host-visible and host-coherent memory properties.
 * - The index data is mapped to the staging buffer and copied using `memcpy`.
 * - A device-local buffer is created with usage flags for transfer destination and index buffer.
 * - The data is transferred from the staging buffer to the device-local buffer using a command buffer.
 * - The staging buffer and its associated memory are cleaned up after the transfer.
 *
 * @note This function assumes that `_indices` contains the index data to be used.
 *
 * @throws Vulkan-related errors if buffer creation, memory mapping, or data transfer fails.
 */
void maverik::vk::RenderingContext::createIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    Utils::CreateBufferProperties stagingBufferProperties = {
        ._logicalDevice = _logicalDevice,
        ._physicalDevice = _physicalDevice,
        ._size = bufferSize,
        ._usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        ._buffer = stagingBuffer,
        ._bufferMemory = stagingBufferMemory
    };

    Utils::createBuffer(stagingBufferProperties);

    void* data;
    vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _indices.data(), (size_t) bufferSize);
    vkUnmapMemory(_logicalDevice, stagingBufferMemory);

    Utils::CreateBufferProperties indexBufferProperties = {
        ._logicalDevice = _logicalDevice,
        ._physicalDevice = _physicalDevice,
        ._size = bufferSize,
        ._usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        ._buffer = _indexBuffer,
        ._bufferMemory = _indexBufferMemory
    };
    Utils::createBuffer(indexBufferProperties);

    Utils::CopyBufferProperties copyBufferProperties = {
        ._logicalDevice = _logicalDevice,
        ._commandPool = _commandPool,
        ._graphicsQueue = _graphicsQueue,
        ._srcBuffer = stagingBuffer,
        ._dstBuffer = _indexBuffer,
        ._size = bufferSize
    };
    Utils::copyBuffer(copyBufferProperties);

    vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);
}


/**
 * @brief Allocates and initializes Vulkan command buffers for rendering operations.
 *
 * This function resizes the `_commandBuffers` vector to accommodate the maximum
 * number of frames in flight (`MAX_FRAMES_IN_FLIGHT`) and allocates primary-level
 * command buffers from the Vulkan command pool (`_commandPool`). The allocated
 * command buffers are stored in the `_commandBuffers` vector.
 *
 * @throws std::runtime_error If the Vulkan command buffer allocation fails.
 */
void maverik::vk::RenderingContext::createCommandBuffers()
{
    _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

    if (vkAllocateCommandBuffers(_logicalDevice, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

/**
 * @brief Creates synchronization objects required for rendering operations.
 *
 * This function initializes semaphores and fences used to synchronize the
 * rendering process. It creates the following synchronization objects:
 * - Image available semaphores: Signal when an image is available for rendering.
 * - Render finished semaphores: Signal when rendering is complete.
 * - In-flight fences: Ensure that a frame is not rendered until the previous
 *   frame has finished.
 *
 * Each synchronization object is created for the maximum number of frames
 * that can be in flight simultaneously, defined by `MAX_FRAMES_IN_FLIGHT`.
 *
 * @throws std::runtime_error If any of the synchronization objects fail to
 *         be created.
 */
void maverik::vk::RenderingContext::createSyncObjects()
{
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("Failed to create synchronization objects for a frame !");
        }
    }
}

/////////////////////
// Private methods //
/////////////////////

/**
 * @brief Determines the maximum usable sample count for multisampling.
 *
 * This function queries the physical device properties to determine the 
 * maximum sample count that can be used for both color and depth 
 * framebuffer attachments. It checks the supported sample counts in 
 * descending order of quality (from 64x to 1x) and returns the highest 
 * supported sample count.
 *
 * @return VkSampleCountFlagBits The maximum usable sample count supported 
 *         by the physical device.
 */
VkSampleCountFlagBits maverik::vk::RenderingContext::getMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(_physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}
