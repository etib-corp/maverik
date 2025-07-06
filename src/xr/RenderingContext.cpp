/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#include "xr/RenderingContext.hpp"

maverik::xr::RenderingContext::RenderingContext(XrInstance XRinstance, VkInstance instance, XrSystemId systemID)
    : _XRinstance(XRinstance), _vulkanInstance(instance), _XRsystemID(systemID)
{
    _vulkanContext = std::make_shared<VulkanContext>();
    init();
}

maverik::xr::RenderingContext::~RenderingContext()
{
}

void maverik::xr::RenderingContext::init()
{
    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();

    _vulkanContext->logicalDevice = _logicalDevice;
    _vulkanContext->physicalDevice = _physicalDevice;
    _vulkanContext->graphicsQueue = _graphicsQueue;
    _vulkanContext->commandPool = _commandPool;
    _vulkanContext->graphicsQueueFamilyIndex = Utils::findQueueFamilies(_physicalDevice).graphicsFamily.value();
    _vulkanContext->renderPass = VK_NULL_HANDLE;
}

void maverik::xr::RenderingContext::pickPhysicalDevice()
{
    if (_XRinstance == XR_NULL_HANDLE) {
        std::cerr << "XR instance is not initialized" << std::endl;
        return;
    }

    if (_physicalDevice != VK_NULL_HANDLE) {
        return;
    }
    PFN_xrGetVulkanGraphicsDevice2KHR xrGetVulkanGraphicsDevice2KHR = nullptr;
    XrVulkanGraphicsDeviceGetInfoKHR graphicsDeviceGetInfo{};

    if (xrGetInstanceProcAddr(_XRinstance, "xrGetVulkanGraphicsDevice2KHR",
        reinterpret_cast<PFN_xrVoidFunction *>(&xrGetVulkanGraphicsDevice2KHR)) != XR_SUCCESS) {
            std::cerr << "Failed to get xrGetVulkanGraphicsDevice2KHR function" << std::endl;
            return;
    }
    graphicsDeviceGetInfo.type = XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR;
    graphicsDeviceGetInfo.systemId = _XRsystemID;
    graphicsDeviceGetInfo.vulkanInstance = _vulkanInstance;

    if (xrGetVulkanGraphicsDevice2KHR(_XRinstance, &graphicsDeviceGetInfo, &_physicalDevice) != XR_SUCCESS) {
        std::cerr << "Failed to get Vulkan graphics device" << std::endl;
        return;
    }
}

void maverik::xr::RenderingContext::createLogicalDevice()
{
    if (_physicalDevice == VK_NULL_HANDLE) {
        std::cerr << "Physical device is not initialized" << std::endl;
        return;
    }

    if (_logicalDevice != VK_NULL_HANDLE) {
        return;
    }

    PFN_xrCreateVulkanDeviceKHR xrCreateVulkanDeviceKHR = nullptr;

    if (xrGetInstanceProcAddr(_XRinstance, "xrCreateVulkanDeviceKHR",
        reinterpret_cast<PFN_xrVoidFunction *>(&xrCreateVulkanDeviceKHR)) != XR_SUCCESS) {
            std::cerr << "Failed to get xrCreateVulkanDeviceKHR function" << std::endl;
            return;
    }

    float queuePriority = 0.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    Utils::QueueFamilyIndices queueFamilyIndices = Utils::findQueueFamilies(_physicalDevice);
    if (!queueFamilyIndices.isComplete()) {
        std::cerr << "Queue family indices are not complete" << std::endl;
        return;
    }
    queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &features;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;

    XrVulkanDeviceCreateInfoKHR vulkanDeviceCreateInfo{};
    vulkanDeviceCreateInfo.type = XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR;
    vulkanDeviceCreateInfo.next = &deviceCreateInfo;
    vulkanDeviceCreateInfo.systemId = _XRsystemID;
    vulkanDeviceCreateInfo.createFlags = 0;
    vulkanDeviceCreateInfo.pfnGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanDeviceCreateInfo.vulkanPhysicalDevice = _physicalDevice;
    vulkanDeviceCreateInfo.vulkanCreateInfo = &deviceCreateInfo;
    vulkanDeviceCreateInfo.vulkanAllocator = nullptr;

    VkResult result = VK_SUCCESS;
    if (xrCreateVulkanDeviceKHR(_XRinstance, &vulkanDeviceCreateInfo, &_logicalDevice, &result) != XR_SUCCESS) {
        std::cerr << "Failed to create Vulkan device" << std::endl;
        return;
    }

    if (result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan device: " << result << std::endl;
        return;
    }

    if (vkGetDeviceQueue(_logicalDevice, queueCreateInfo.queueFamilyIndex, 0, &_graphicsQueue) != VK_SUCCESS) {
        std::cerr << "Failed to get Vulkan queue" << std::endl;
        return;
    }
}

void maverik::xr::RenderingContext::createCommandPool()
{
    if (_logicalDevice == VK_NULL_HANDLE) {
        std::cerr << "Logical device is not initialized" << std::endl;
        return;
    }

    if (_commandPool != VK_NULL_HANDLE) {
        return;
    }

    Utils::QueueFamilyIndices queueCreateInfo = Utils::findQueueFamilies(_physicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueCreateInfo.graphicsFamily.value();
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(_logicalDevice, &commandPoolCreateInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan command pool" << std::endl;
        return;
    }
}

void maverik::xr::RenderingContext::createGraphicsPipeline(VkRenderPass renderPass)
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
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr; // Viewports will be set dynamically
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr; // Scissors will be set dynamically

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.rasterizationSamples = getMaxUsableSampleCount();

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
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
    pipelineLayoutInfo.setLayoutCount = 0; // No descriptor set layout for now
    pipelineLayoutInfo.pushConstantRangeCount = 0; // No push constants for now
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(_logicalDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        std::cerr << "Failed to create pipeline layout" << std::endl;
        return;
    }

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

    if (vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
        std::cerr << "Failed to create graphics pipeline" << std::endl;
        return;
    }
}
