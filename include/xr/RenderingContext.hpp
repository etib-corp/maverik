/*
** ETIB PROJECT, 2025
** maverik
** File description:
** RenderingContext
*/

#pragma once

#include "xr/Openxr-include.hpp"
#include "ARenderingContext.hpp"

#include "Utils.hpp"

namespace maverik {
    namespace xr {

        struct RenderingContextPropertiesXR {
            XrInstance _XRinstance;
            XrSystemId _XRsystemID;
            VkInstance _vulkanInstance;
        };

        class RenderingContext : public maverik::ARenderingContext {
            public:
                RenderingContext(const RenderingContextPropertiesXR &properties);
                ~RenderingContext() override;

                void init() override;

            protected:

                void pickPhysicalDevice(VkInstance instance) override;

                void createLogicalDevice() override;

                void createCommandPool() override;

                void createRenderPass() override;

                void createGraphicsPipeline(VkRenderPass renderPass) override;

            private:
                XrInstance _XRinstance = XR_NULL_HANDLE;
                XrSystemId _XRsystemID = XR_NULL_SYSTEM_ID;
                VkInstance _vulkanInstance = VK_NULL_HANDLE;

                VkPipelineLayout _pipelineLayout;       // Vulkan pipeline layout
                VkPipeline _graphicsPipeline;           // Vulkan graphics pipeline
        };
    }
}
