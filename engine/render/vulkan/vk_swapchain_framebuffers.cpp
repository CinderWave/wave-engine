#include "vk_swapchain_framebuffers.hpp"

#include <utility>  // std::exchange

namespace wave::engine::render::vulkan {

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

SwapchainFramebuffers::~SwapchainFramebuffers() {
    destroy();
}

SwapchainFramebuffers::SwapchainFramebuffers(SwapchainFramebuffers&& other) noexcept {
    m_device       = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_framebuffers = std::move(other.m_framebuffers);
}

SwapchainFramebuffers& SwapchainFramebuffers::operator=(SwapchainFramebuffers&& other) noexcept {
    if (this != &other) {
        destroy();

        m_device       = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_framebuffers = std::move(other.m_framebuffers);
    }
    return *this;
}

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

void SwapchainFramebuffers::create(
    VkDevice device,
    VkRenderPass renderPass,
    const std::vector<VkImageView>& swapchainImageViews,
    VkImageView depthView,
    std::uint32_t width,
    std::uint32_t height
) {
    destroy();

    m_device = device;

    const bool hasDepth = (depthView != VK_NULL_HANDLE);
    const std::uint32_t imageCount =
        static_cast<std::uint32_t>(swapchainImageViews.size());

    m_framebuffers.resize(imageCount);

    for (std::uint32_t i = 0; i < imageCount; ++i) {
        // Attachments per framebuffer:
        //  - [0] swapchain color image view
        //  - [1] optional shared depth view
        VkImageView attachments[2];
        std::uint32_t attachmentCount = 1;

        attachments[0] = swapchainImageViews[i];

        if (hasDepth) {
            attachments[1] = depthView;
            attachmentCount = 2;
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = attachmentCount;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = width;
        framebufferInfo.height          = height;
        framebufferInfo.layers          = 1;

        vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffers[i]);
    }
}

void SwapchainFramebuffers::destroy() {
    if (m_device != VK_NULL_HANDLE) {
        for (VkFramebuffer fb : m_framebuffers) {
            if (fb != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(m_device, fb, nullptr);
            }
        }
    }

    m_framebuffers.clear();
    m_device = VK_NULL_HANDLE;
}

} // namespace wave::engine::render::vulkan
