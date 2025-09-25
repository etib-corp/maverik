/*
** EPITECH PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#include "vk/GraphicalContext.hpp"

//////////////////////
// Static functions //
//////////////////////

static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

////////////////////
// Public methods //
////////////////////

/*
** Construct a new GraphicalContext object.
**
** This constructor is used to create a new GraphicalContext object in a vulkan native context.
*/
maverik::vk::GraphicalContext::GraphicalContext()
{
    _appName = "Hello, World !";
    _appVersion = new Version(1, 0, 0);
    _engineName = "Maverik";
    _engineVersion = new Version(1, 0, 0);

    this->createInstance();

    maverik::vk::RenderingContext::RenderingContextProperties renderingContextProperties = {
        ._instance = _instance,
        ._textureImageViewsAndSamplers = {}
    };
    maverik::vk::RenderingContext::WindowProperties windowProperties = {
        .width = 800,
        .height = 600,
        .title = _appName
    };
    _renderingContext = std::make_shared<maverik::vk::RenderingContext>(windowProperties, renderingContextProperties);
}

/*
** Construct a new GraphicalContext object.
**
** This constructor is used to create a new GraphicalContext object in a vulkan native context.
** It initializes the application name, application version, engine name and engine version.
**
** @param appName the application name
** @param appVersion the application version
** @param engineName the engine name
** @param engineVersion the engine version
**
*/
maverik::vk::GraphicalContext::GraphicalContext(const std::string &appName, const Version &appVersion, const std::string &engineName, const Version &engineVersion, unsigned int windowWidth = 800, unsigned int windowHeight = 600)
{
    _appName = appName;
    _appVersion = new Version(appVersion);
    _engineName = engineName;
    _engineVersion = new Version(engineVersion);

    this->createInstance();

        maverik::vk::RenderingContext::RenderingContextProperties renderingContextProperties = {
        ._instance = _instance,
        ._textureImageViewsAndSamplers = {}
    };
    maverik::vk::RenderingContext::WindowProperties windowProperties = {
        .width = 800,
        .height = 600,
        .title = _appName
    };
    _renderingContext = std::make_shared<maverik::vk::RenderingContext>(windowProperties, renderingContextProperties);
}

/*
** Destructor for the GraphicalContext class.
*/
maverik::vk::GraphicalContext::~GraphicalContext()
{
}

/*
** Create the Vulkan instance.
**
** This function is used to create the Vulkan instance in a vulkan native context.
** It initializes the application name, application version, engine name and engine version (into the vulkan instance).
**
** @return void
*/
void maverik::vk::GraphicalContext::createInstance()
{
    if (enableValidationLayers && !this->checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = _appName.c_str();
    appInfo.applicationVersion = _appVersion->to_uint32_t();
    appInfo.pEngineName = _engineName.c_str();
    appInfo.engineVersion = _engineVersion->to_uint32_t();
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = this->getInstanceExtensions();
    // Because the extensions are stored as std::string, we need to convert them to const char*
    // to pass them to the Vulkan API.
    auto extensionsWrapped = std::vector<const char*>(extensions.size());

    std::transform(extensions.begin(), extensions.end(), extensionsWrapped.begin(), [](const std::string &str) {
        return str.c_str();
    });
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsWrapped.size());
    createInfo.ppEnabledExtensionNames = extensionsWrapped.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    this->populateDebugMessengerCreateInfo(debugCreateInfo, defaultDebugCallback);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
        std::cout << "Failed to create instance due to incompatible driver ! Trying with MacOS settings..." << std::endl;

        extensionsWrapped.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensionsWrapped.emplace_back("VK_KHR_portability_enumeration");

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        createInfo.enabledExtensionCount = (uint32_t) extensionsWrapped.size();
        createInfo.ppEnabledExtensionNames = extensionsWrapped.data();

        result = vkCreateInstance(&createInfo, nullptr, &_instance);
        switch (result) {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                throw std::runtime_error("Host out of memory !");
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                throw std::runtime_error("Device out of memory !");
            case VK_ERROR_INITIALIZATION_FAILED:
                throw std::runtime_error("Initialization failed !");
            case VK_ERROR_LAYER_NOT_PRESENT:
                throw std::runtime_error("Layer not present !");
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                throw std::runtime_error("Extension not present !");
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                throw std::runtime_error("Incompatible driver !");
            default:
                break;
        }
    }
    if (result == VK_SUCCESS) {
        std::cout << "Instance created !" << std::endl;
    } else {
        throw std::runtime_error("Failed to create instance !");
    }
}


/*
** Get the required extensions for the instance.
**
** On MacOS, the VK_KHR_portability_enumeration extension is required.
**
** @return the required extensions for the instance
*/
std::vector<std::string> maverik::vk::GraphicalContext::getInstanceExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back("VK_KHR_portability_enumeration");
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return std::vector<std::string>(extensions.begin(), extensions.end());
}

/////////////////////
// Private methods //
/////////////////////

/*
** Populate the debug messenger create info.
**
** This function is used to create the debug messenger for the instance.
** It is used to log the debug messages from the Vulkan API.
**
** @param createInfo the debug messenger create info
**
** @return void
*/
void maverik::vk::GraphicalContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo, debugCallback_t debugCallback)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

/*
** Check if the validation layers are available on the system.
**
** Be careful on MacOS, the validation layers are not available by default.
** You need to install them manually or use MoltenVK.
**
** @return true if the validation layers are available, false otherwise
*/
bool maverik::vk::GraphicalContext::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }
    return true;
}

