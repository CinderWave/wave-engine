#include "vk_forward_render_pass.hpp"

#include <utility> // std::exchange

namespace wave::engine::render::vulkan {

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

ForwardRenderPass::~ForwardRenderPass() {
    destroy();
}

ForwardRenderPass::ForwardRenderPass(ForwardRenderPass&& other) noexcept {
    m_device      = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_renderPass  = std::exchange(other.m_renderPass, VK_NULL_HANDLE);
}

ForwardRenderPass& ForwardRenderPass::operator=(ForwardRenderPass&& other) noexcept {
    if (this != &other) {
        destroy();

        m_device     = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_renderPass = std::exchange(other.m_renderPass, VK_NULL_HANDLE);
    }
    return *this;
}

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

void ForwardRenderPass::create(
    VkDevice device,
    VkFormat colorFormat,
    VkFormat depthFormat
) {
    destroy();

    m_device = device;

    // Attachments --------------------------------------------------------------
    // Color attachment
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = colorFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // index in attachment array
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Optional depth attachment
    VkAttachmentDescription depthAttachment{};
    VkAttachmentReference  depthAttachmentRef{};
    bool hasDepth = (depthFormat != VK_FORMAT_UNDEFINED);

    if (hasDepth) {
        depthAttachment.format         = depthFormat;
        depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        depthAttachmentRef.attachment = 1; // after color
        depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    // Subpass ------------------------------------------------------------------
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = hasDepth ? &depthAttachmentRef : nullptr;

    // Subpass dependency to ensure correct layout transitions ------------------
    VkSubpassDependency dependency{};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Attachment array ---------------------------------------------------------
    VkAttachmentDescription attachments[2];
    std::uint32_t attachmentCount = 1;

    attachments[0] = colorAttachment;
    if (hasDepth) {
        attachments[1] = depthAttachment;
        attachmentCount = 2;
    }

    // Render pass create info --------------------------------------------------
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachmentCount;
    renderPassInfo.pAttachments    = attachments;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass);
}

void ForwardRenderPass::destroy() {
    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }

    m_device = VK_NULL_HANDLE;
}

} // namespace wave::engine::render::vulkan
