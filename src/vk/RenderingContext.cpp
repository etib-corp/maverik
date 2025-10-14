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

    // Initialize VulkanContext (used to setup the rest of the engine)
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

maverik::vk::RenderingContext::~RenderingContext()
{
}

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
