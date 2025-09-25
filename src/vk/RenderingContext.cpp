/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#include "RenderingContext.hpp"

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
maverik::vk::RenderingContext::RenderingContext(const WindowProperties &windowProperties, const RenderingContextProperties &renderingContextProperties)
{
    this->initWindow(windowProperties.width, windowProperties.height, windowProperties.title);
    this->createSurface(renderingContextProperties._instance);
    this->pickPhysicalDevice(renderingContextProperties._instance);
    this->createLogicalDevice();
    this->createDescriptorSetLayout();
    this->createRenderPass(_physicalDevice, _logicalDevice, _surface, _msaaSamples);
    this->createGraphicsPipeline(_renderPass);
    this->createCommandPool();
    this->createVertexBuffer();
    this->createIndexBuffer();
    this->createUniformBuffers();
    this->createDescriptorPool();
    this->createDescriptorSets(renderingContextProperties._textureImageViewsAndSamplers);
    this->createCommandBuffers();
    this->createSyncObjects();
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
 * @brief Creates a Vulkan graphics pipeline for rendering.
 *
 * This function sets up and creates a Vulkan graphics pipeline, which includes
 * configuring shader stages, vertex input, input assembly, viewport, rasterization,
 * multisampling, color blending, depth and stencil testing, and dynamic states.
 *
 * @param renderPass The Vulkan render pass object that the pipeline will be compatible with.
 *
 * @throws std::runtime_error If the pipeline layout or graphics pipeline creation fails.
 *
 * The function performs the following steps:
 * - Reads and compiles vertex and fragment shader modules.
 * - Configures shader stages using the compiled shader modules.
 * - Sets up vertex input state, including binding and attribute descriptions.
 * - Configures input assembly state for primitive topology.
 * - Sets up viewport and scissor states.
 * - Configures rasterization state, including culling and polygon mode.
 * - Sets up multisampling state for anti-aliasing.
 * - Configures color blending state for framebuffer output.
 * - Sets up dynamic states for viewport and scissor.
 * - Creates a pipeline layout using descriptor set layouts.
 * - Configures depth and stencil testing state.
 * - Creates the graphics pipeline using all the configured states.
 * - Cleans up shader modules after pipeline creation.
 */
void maverik::vk::RenderingContext::createGraphicsPipeline(VkRenderPass renderPass)
{
    auto vertShaderCode = Utils::readFile("shaders/vert.spv");
    auto fragShaderCode = Utils::readFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = Utils::createShaderModule(_logicalDevice, vertShaderCode);
    VkShaderModule fragShaderModule = Utils::createShaderModule(_logicalDevice, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = _msaaSamples;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.minSampleShading = .2f;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

    if (vkCreatePipelineLayout(_logicalDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout !");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pDepthStencilState = &depthStencil;

    if (vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline !");
    }

    vkDestroyShaderModule(_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(_logicalDevice, vertShaderModule, nullptr);
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
 * @brief Creates uniform buffers for the rendering context.
 *
 * This function initializes and allocates uniform buffers for each frame in flight.
 * It creates a buffer and allocates memory for each frame, ensuring that the buffers
 * are host-visible and host-coherent. The memory for each buffer is also mapped for
 * easy access during rendering operations.
 *
 * @details
 * - The size of each uniform buffer is determined by the size of the `UniformBufferObject`.
 * - The number of uniform buffers created corresponds to the `MAX_FRAMES_IN_FLIGHT` constant.
 * - The buffers are created with the `VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT` usage flag.
 * - The memory properties used are `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` and
 *   `VK_MEMORY_PROPERTY_HOST_COHERENT_BIT` to allow CPU access and ensure memory coherency.
 * - The memory for each buffer is mapped immediately after allocation for future use.
 *
 * @throws std::runtime_error If buffer creation or memory mapping fails.
 */
void maverik::vk::RenderingContext::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        Utils::CreateBufferProperties bufferProperties = {
            ._logicalDevice = _logicalDevice,
            ._physicalDevice = _physicalDevice,
            ._size = bufferSize,
            ._usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            ._buffer = _uniformBuffers[i],
            ._bufferMemory = _uniformBuffersMemory[i]
        };

        Utils::createBuffer(bufferProperties);
        vkMapMemory(_logicalDevice, _uniformBuffersMemory[i], 0, bufferSize, 0, &_uniformBuffersMapped[i]);
    }
}

/**
 * @brief Sets up the Vulkan debug messenger for the rendering context.
 *
 * This function initializes a Vulkan debug messenger if validation layers
 * are enabled. The debug messenger is used to capture and handle debug
 * messages from the Vulkan API, which can help in identifying issues
 * during development.
 *
 * @param instance The Vulkan instance to associate the debug messenger with.
 *
 * @throws std::runtime_error If the debug messenger setup fails.
 */
void maverik::vk::RenderingContext::setupDebugMessenger(VkInstance instance)
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;

    Utils::populateDebugMessengerCreateInfo(createInfo, defaultDebugCallback);
    if (Utils::createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger !");
    }
}

/**
 * @brief Creates a Vulkan descriptor pool for managing descriptor sets.
 *
 * This function initializes a descriptor pool with specific pool sizes for
 * uniform buffers and combined image samplers. The number of descriptors
 * allocated for each type is determined by the constant MAX_FRAMES_IN_FLIGHT.
 *
 * @throws std::runtime_error If the Vulkan descriptor pool creation fails.
 *
 * The descriptor pool is used to allocate descriptor sets, which are
 * essential for binding resources (e.g., buffers and images) to shaders
 * during rendering.
 */
void maverik::vk::RenderingContext::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

/**
 * @brief Creates the descriptor set layout for the Vulkan rendering context.
 *
 * This function defines and creates a descriptor set layout that specifies
 * the bindings for uniform buffers and combined image samplers. The layout
 * is used to interface between shaders and resources such as uniform buffers
 * and textures.
 *
 * The descriptor set layout includes:
 * - A uniform buffer binding at binding index 0, accessible in the vertex shader stage.
 * - A combined image sampler binding at binding index 1, accessible in the fragment shader stage.
 *
 * @throws std::runtime_error If the Vulkan API call to create the descriptor set layout fails.
 */
void maverik::vk::RenderingContext::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_logicalDevice, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

/**
 * @brief Creates and allocates descriptor sets for the rendering context.
 *
 * This function sets up descriptor sets for each frame in flight, binding
 * uniform buffers and texture samplers to the appropriate descriptor bindings.
 *
 * @param textureImageView The Vulkan image view representing the texture to be sampled.
 * @param textureSampler The Vulkan sampler used for sampling the texture.
 *
 * @throws std::runtime_error If descriptor set allocation fails.
 *
 * The function performs the following steps:
 * 1. Allocates descriptor sets from the descriptor pool for the maximum number of frames in flight.
 * 2. Configures descriptor buffer info for uniform buffers.
 * 3. Configures descriptor image info for the texture image view and sampler.
 * 4. Updates the descriptor sets with the uniform buffer and texture sampler bindings.
 */
void maverik::vk::RenderingContext::createDescriptorSets(std::map<VkImageView, VkSampler> imagesViewsAndSamplers)
{
    for (const auto [imageView, sampler] : imagesViewsAndSamplers) {
        this->createSingleDescriptorSets(imageView, sampler);
    }
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

/**
 * @brief Creates and allocates a single descriptor sets for the rendering context.
 *
 * This function sets up a single descriptor sets for a single texture image view and a single texture sampler
 * for each frame in flight, binding uniform buffers and texture samplers to the appropriate descriptor bindings.
 *
 * @param textureImageView The Vulkan image view representing the texture to be sampled.
 * @param textureSampler The Vulkan sampler used for sampling the texture.
 *
 * @throws std::runtime_error If descriptor set allocation fails.
 *
 * The function performs the following steps:
 * 1. Allocates descriptor sets from the descriptor pool for the maximum number of frames in flight.
 * 2. Configures descriptor buffer info for uniform buffers.
 * 3. Configures descriptor image info for the texture image view and sampler.
 * 4. Updates the descriptor sets with the uniform buffer and texture sampler bindings.
 */
void maverik::vk::RenderingContext::createSingleDescriptorSets(VkImageView textureImageView, VkSampler textureSampler)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(_logicalDevice, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets !");
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(_logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
