#pragma once

#include "engine/render/render_backend.hpp"

#include <vulkan/vulkan.h>
#include <string>
#include <cstdint>
#include <vector>

#include "engine/render/vulkan/vk_pipeline.hpp"

namespace wave::engine::render::vulkan
{
    class VkBackend : public RenderBackend
    {
    public:
        VkBackend() = default;
        ~VkBackend() override;

        bool initialize(const RenderInitInfo& info) override;
        void begin_frame() override;
        void end_frame() override;
        void resize(std::uint32_t width, std::uint32_t height) override;
        void shutdown() override;

    private:
        // Platform / window handle (GLFWwindow* in our current setup)
        void* m_window_handle = nullptr;

        // Vulkan core handles
        VkInstance       m_instance        = VK_NULL_HANDLE;
        VkSurfaceKHR     m_surface         = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice  = VK_NULL_HANDLE;
        VkDevice         m_device          = VK_NULL_HANDLE;

        // Swapchain
        VkSwapchainKHR           m_swapchain            = VK_NULL_HANDLE;
        VkFormat                 m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D               m_swapchainExtent{};
        std::vector<VkImage>     m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;

        // Render pass + framebuffers
        VkRenderPass               m_renderPass = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> m_framebuffers;

        // Command pool + command buffers (one per frame in flight)
        VkCommandPool                m_commandPool   = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_commandBuffers;

        // Queues
        VkQueue       m_graphicsQueue        = VK_NULL_HANDLE;
        VkQueue       m_presentQueue         = VK_NULL_HANDLE;
        std::uint32_t m_graphicsQueueFamily  = UINT32_MAX;
        std::uint32_t m_presentQueueFamily   = UINT32_MAX;

        // Sync
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence>     m_inFlightFences;
        std::uint32_t            m_currentFrame      = 0;
        std::uint32_t            m_currentImageIndex = 0;

        // Simple graphics pipeline used in the default pass
        VkSimplePipeline m_simplePipeline;

        std::uint32_t m_width  = 0;
        std::uint32_t m_height = 0;

        // For debugging names
        std::string m_debug_name;

        // Swapchain helpers
        bool create_swapchain();
        void destroy_swapchain();

        bool create_render_pass();
        void destroy_render_pass();

        bool create_framebuffers();
        void destroy_framebuffers();

        bool create_command_pool();
        void destroy_command_pool();

        bool create_command_buffers();
        void destroy_command_buffers();

        bool create_sync_objects();
        void destroy_sync_objects();

        bool acquire_next_image();
        bool record_command_buffer(VkCommandBuffer cmd, uint32_t imageIndex);
        bool present_image();
    };

} // namespace wave::engine::render::vulkan
