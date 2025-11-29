#pragma once

#include <vulkan/vulkan.h>

namespace wave::engine::render::vulkan {

// Simple RAII wrapper around a forward render pass with
// one color attachment and an optional depth attachment.
//
// Intended for the editor viewport and early engine rendering.
class ForwardRenderPass final {
public:
    ForwardRenderPass() = default;
    ~ForwardRenderPass();

    ForwardRenderPass(const ForwardRenderPass&) = delete;
    ForwardRenderPass& operator=(const ForwardRenderPass&) = delete;

    ForwardRenderPass(ForwardRenderPass&& other) noexcept;
    ForwardRenderPass& operator=(ForwardRenderPass&& other) noexcept;

    // Create a render pass with:
    //  - colorFormat  as the swapchain color format
    //  - depthFormat  as the depth attachment format (VK_FORMAT_UNDEFINED = no depth)
    //
    // Layouts:
    //  - Color: UNDEFINED   -> PRESENT_SRC_KHR
    //  - Depth: UNDEFINED   -> DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    void create(
        VkDevice device,
        VkFormat colorFormat,
        VkFormat depthFormat
    );

    void destroy();

    VkRenderPass handle() const { return m_renderPass; }

private:
    VkDevice     m_device{VK_NULL_HANDLE};
    VkRenderPass m_renderPass{VK_NULL_HANDLE};
};

} // namespace wave::engine::render::vulkan
