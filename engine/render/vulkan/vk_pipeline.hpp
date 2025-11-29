#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <string_view>

#include "engine/render/vulkan/vk_shader.hpp"

namespace wave::engine::render::vulkan
{
    // A minimal graphics pipeline intended for:
    //  - "clear + triangle" style tests
    //  - simple editor passes
    //
    // Assumptions:
    //  - No vertex buffers (vertex shader generates geometry procedurally)
    //  - Single subpass render pass
    //  - Single color attachment
    //
    class VkSimplePipeline
    {
    public:
        VkSimplePipeline() = default;
        ~VkSimplePipeline();

        VkSimplePipeline(const VkSimplePipeline&)            = delete;
        VkSimplePipeline& operator=(const VkSimplePipeline&) = delete;

        VkSimplePipeline(VkSimplePipeline&& other) noexcept;
        VkSimplePipeline& operator=(VkSimplePipeline&& other) noexcept;

        // Create graphics pipeline using the provided render pass and swapchain extent.
        //
        // vertResource / fragResource are resource-relative paths, e.g.:
        //   "shaders/editor/simple.vert.spv"
        //   "shaders/editor/simple.frag.spv"
        //
        bool initialize(VkDevice device,
                        VkRenderPass renderPass,
                        VkExtent2D swapchainExtent,
                        std::string_view vertResource,
                        std::string_view fragResource);

        void shutdown() noexcept;

        bool is_valid() const noexcept { return m_pipeline != VK_NULL_HANDLE; }

        VkPipeline       handle() const noexcept { return m_pipeline; }
        VkPipelineLayout layout() const noexcept { return m_pipelineLayout; }

    private:
        void destroy() noexcept;

        VkDevice         m_device         = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipeline       m_pipeline       = VK_NULL_HANDLE;

        VkShaderModuleHandle m_vertShader;
        VkShaderModuleHandle m_fragShader;
    };

} // namespace wave::engine::render::vulkan
