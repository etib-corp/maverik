/*
** ETIB PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#pragma once

#include <iostream>

#include "AGraphicalContext.hpp"
#include "vk/RenderingContext.hpp"

#include "Version.hpp"

namespace maverik {
    namespace vk {
        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            "VK_KHR_portability_subset"
        };

        #ifdef NDEBUG
            const bool enableValidationLayers = false;
        #else
            const bool enableValidationLayers = true;
        #endif

        class GraphicalContext : public maverik::AGraphicalContext {
            public:
                /*
                ** @brief Callback type for the debug messenger.
                **
                ** This callback is used to handle debug messages from the Vulkan API.
                ** It is used to log the debug messages from the Vulkan API.
                **
                ** @param severity the severity of the message
                ** @param type the type of the message
                ** @param pCallbackData the callback data
                ** @param pUserData the user data
                **
                ** @return true if the message is handled, false otherwise
                */
                typedef VkBool32 debugCallback_t(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);

                // Constructors
                GraphicalContext();
                GraphicalContext(const std::string &appName, const Version &appVersion, const std::string &engineName, const Version &engineVersion, unsigned int windowWidth = 800, unsigned int windowHeight = 600);
                GraphicalContext(const GraphicalContext &other) = delete; // Copy constructor

                // Destructor
                ~GraphicalContext();

                std::vector<std::string> getInstanceExtensions() override;

                void createInstance() override;

            private:
                //////////////////
                /// Properties ///
                //////////////////
                std::string _appName;       // Application name
                Version *_appVersion;       // Application version (major, minor, patch)
                std::string _engineName;    // Engine name
                Version *_engineVersion;    // Engine version (major, minor, patch)

                // Validation layers & extensions
                bool checkValidationLayerSupport();

                // Debug messenger
                void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo, debugCallback_t debugCallback);

        };
    }
}
