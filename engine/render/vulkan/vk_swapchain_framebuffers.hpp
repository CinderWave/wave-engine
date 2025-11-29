#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace wave::engine::render::vulkan {

// Helper for creating one framebuffer per swapchain image view,
// with an optional shared depth attachment.
class SwapchainFramebuffers final {
public:
    SwapchainFramebuffers() = default;
    ~SwapchainFramebuffers();

    SwapchainFramebuffers(const SwapchainFramebuffers&) = delete;
    SwapchainFramebuffers& operator=(const SwapchainFramebuffers&) = delete;

    SwapchainFramebuffers(SwapchainFramebuffers&& other) noexcept;
    SwapchainFramebuffers& operator=(SwapchainFramebuffers&& other) noexcept;

    // Create framebuffers:
    //  - one per swapchain image view
    //  - each using the provided render pass
    //  - optional depthView (VK_NULL_HANDLE = no depth attachment)
    void create(
        VkDevice device,
        VkRenderPass renderPass,
        const std::vector<VkImageView>& swapchainImageViews,
        VkImageView depthView,
        std::uint32_t width,
        std::uint32_t height
    );

    void destroy();

    VkFramebuffer framebuffer(std::uint32_t index) const {
        return m_framebuffers[index];
    }

    std::uint32_t count() const {
        return static_cast<std::uint32_t>(m_framebuffers.size());
    }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    std::vector<VkFramebuffer> m_framebuffers;
};

} // namespace wave::engine::render::vulkan
