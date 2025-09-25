/*
** ETIB PROJECT, 2025
** maverik
** File description:
** SwapchainContext
*/

#ifndef SWAPCHAINCONTEXT_HPP_
#define SWAPCHAINCONTEXT_HPP_

#include"maverik.hpp"
#include "ASwapchainContext.hpp"
#include "Utils.hpp"
#include "xr/Openxr-include.hpp"


namespace maverik {
    namespace xr {

        struct SwapchainContextCreationPropertiesXR {
            XrInstance _instance;
            XrSystemId _systemId;
            XrSession _session;
            VkPhysicalDevice _physicalDevice;
            VkDevice _device;
        };

        struct SwapchainImageCreationPropertiesXR {
            VkDevice _device;
            VkPhysicalDevice _physicalDevice;
            uint32_t _capacity;
            XrSwapchainCreateInfo _swapchainCreateInfo;

        };

        struct SwapChainImage {

            VkFormat _swapchainImageFormat = VK_FORMAT_UNDEFINED;
            VkExtent2D _swapchainExtent;
            std::vector<XrSwapchainImageVulkan2KHR> _swapchainImages;
            std::vector<VkImageView> _swapchainImageViews;

            std::vector<VkFramebuffer> _swapchainFrameBuffers;

            VkImage _colorImage = VK_NULL_HANDLE;
            VkDeviceMemory _colorImageMemory = VK_NULL_HANDLE;
            VkImageView _colorImageView = VK_NULL_HANDLE;

            VkImage _depthImage = VK_NULL_HANDLE;
            VkDeviceMemory _depthImageMemory = VK_NULL_HANDLE;
            VkImageView _depthImageView = VK_NULL_HANDLE;

            std::vector<VkCommandBuffer> _graphicsCommandBuffers;
            const uint32_t _maxFramesInFlight = 2;

            std::vector<VkFence> _inFlightFences;
            std::vector<VkFence> _imagesInFlight;

            XrSwapchainImageBaseHeader* _image;

            uint32_t _currentFame = 0;

            VkViewport _viewport = {0, 0, 0, 0, 0, 1.0};
            VkRect2D _scissor = {{0, 0}, {0, 0}};

            VkDevice _device;
            VkPhysicalDevice _physicalDevice;

            void createColorResources();

            void createDepthResources();

            void createFrameBuffers();

            // void createCommandBuffers();

            // void createSyncObjects();

            void init(const SwapchainImageCreationPropertiesXR& properties);

            // XrSwapchainImageBaseHeader *getFirstImagePointer();

            bool _initialized = false;
        };

        class SwapchainContext : public ASwapchainContext {
            public:
                SwapchainContext(const SwapchainContextCreationPropertiesXR& properties);
                ~SwapchainContext();

            protected:
                void init();

                void initSwapchainImages();

                uint64_t selectSwapchainFormat(const std::vector<int64_t> &swapchainFormats);

                std::shared_ptr<SwapChainImage>  createSwapchainImage(XrSwapchainCreateInfo& swapchainCreateInfo, ASwapchain<XrSwapchain> &swapchain);

            private:
                XrInstance _instance;
                XrSystemId _systemId;
                XrSession _session;
                VkPhysicalDevice _physicalDevice;
                VkDevice _device;

                std::vector<XrViewConfigurationView> _viewsConfigurations;
                std::vector<XrView> _views;

                std::map<XrSwapchain, std::shared_ptr<SwapChainImage>> _swapchainImages;
        };

    }
}

#endif /* !SWAPCHAINCONTEXT_HPP_ */
