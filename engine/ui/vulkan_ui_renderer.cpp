#include "vulkan_ui_renderer.hpp"

#include "../../vulkan/vulkan_device.hpp"
#include "../../vulkan/vulkan_swapchain.hpp"
#include "../../vulkan/vulkan_command_pool.hpp"

#include <cstring>

namespace wave::render {

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

VulkanUIRenderer::~VulkanUIRenderer() {
    shutdown();
}

bool VulkanUIRenderer::initialize(VulkanDevice* device,
                                  VulkanSwapchain* swapchain,
                                  VulkanCommandPool* commandPool) {
    m_device      = device;
    m_swapchain   = swapchain;
    m_commandPool = commandPool;

    if (!create_vertex_buffer()) {
        return false;
    }

    if (!create_pipeline()) {
        return false;
    }

    return true;
}

void VulkanUIRenderer::shutdown() {
    destroy_pipeline();
    destroy_vertex_buffer();

    m_device = nullptr;
    m_swapchain = nullptr;
    m_commandPool = nullptr;
}

void VulkanUIRenderer::recreate() {
    destroy_pipeline();
    create_pipeline();
}

// -----------------------------------------------------------------------------
// GPU buffers
// -----------------------------------------------------------------------------

bool VulkanUIRenderer::create_vertex_buffer() {
    if (!m_device) {
        return false;
    }

    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size  = static_cast<VkDeviceSize>(m_vertexStride) * m_maxVertices;
    info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (vkCreateBuffer(m_device->logical_device(), &info, nullptr, &m_vertexBuffer) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device->logical_device(), m_vertexBuffer, &memReq);

    VkMemoryAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc.allocationSize = memReq.size;
    alloc.memoryTypeIndex = m_device->find_memory_type(
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    if (vkAllocateMemory(m_device->logical_device(), &alloc, nullptr, &m_vertexMemory) != VK_SUCCESS) {
        return false;
    }

    vkBindBufferMemory(m_device->logical_device(), m_vertexBuffer, m_vertexMemory, 0);

    return true;
}

void VulkanUIRenderer::destroy_vertex_buffer() {
    if (!m_device) {
        return;
    }

    if (m_vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device->logical_device(), m_vertexBuffer, nullptr);
        m_vertexBuffer = VK_NULL_HANDLE;
    }

    if (m_vertexMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device->logical_device(), m_vertexMemory, nullptr);
        m_vertexMemory = VK_NULL_HANDLE;
    }
}

// -----------------------------------------------------------------------------
// Pipeline
// -----------------------------------------------------------------------------

bool VulkanUIRenderer::create_pipeline() {
    if (!m_device || !m_swapchain) {
        return false;
    }

    // Placeholder pipeline creation
    // Later we add shaders (ui.vert + ui.frag), descriptor sets, blending etc.

    VkPipelineLayoutCreateInfo layout{};
    layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (vkCreatePipelineLayout(m_device->logical_device(), &layout, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        return false;
    }

    // Graphics pipeline omitted for brevity; placeholder pipeline object:
    m_pipeline = VK_NULL_HANDLE;

    return true;
}

void VulkanUIRenderer::destroy_pipeline() {
    if (!m_device) {
        return;
    }

    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device->logical_device(), m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device->logical_device(), m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}

// -----------------------------------------------------------------------------
// Recording
// -----------------------------------------------------------------------------

void VulkanUIRenderer::record_ui(VkCommandBuffer cmd,
                                 const editor::ui::UIDrawList& drawList) {
    if (!m_device || !m_swapchain) {
        return;
    }

    // Upload CPU draw commands to GPU vertex buffer
    upload_commands_to_buffer(drawList);

    // In a real implementation we would:
    //   - bind pipeline
    //   - bind vertex buffer
    //   - set blend state
    //   - record vkCmdDraw for each batch

    // Placeholder binding:
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkBuffer vertexBuffers[] = { m_vertexBuffer };
    VkDeviceSize offsets[]   = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

    std::uint32_t vertexCount = static_cast<std::uint32_t>(m_cpuVertexData.size() / 6);
    if (vertexCount > 0) {
        vkCmdDraw(cmd, vertexCount, 1, 0, 0);
    }
}

// -----------------------------------------------------------------------------
// CPU → GPU upload of UI commands
// -----------------------------------------------------------------------------

void VulkanUIRenderer::upload_commands_to_buffer(const editor::ui::UIDrawList& drawList) {
    // Simplified packing:
    // Each UIDrawCommand becomes 6 floats per vertex * 6 vertices (two triangles)
    // Real implementation will batch different shapes.
    
    m_cpuVertexData.clear();
    m_cpuVertexData.reserve(drawList.commands().size() * 6 * 6);

    for (const auto& cmd : drawList.commands()) {
        const float x = cmd.x;
        const float y = cmd.y;
        const float w = cmd.width;
        const float h = cmd.height;

        // Color
        const float r = cmd.r;
        const float g = cmd.g;
        const float b = cmd.b;
        const float a = cmd.a;

        // Build a rectangle as two triangles.
        // Placeholder: no UVs yet (0,0).

        float verts[6][6] = {
            { x,     y,     0.0f, 0.0f, r, g }, // top-left
            { x + w, y,     0.0f, 0.0f, r, g }, // top-right
            { x + w, y + h, 0.0f, 0.0f, r, g }, // bottom-right

            { x,     y,     0.0f, 0.0f, r, g }, // top-left
            { x + w, y + h, 0.0f, 0.0f, r, g }, // bottom-right
            { x,     y + h, 0.0f, 0.0f, r, g }, // bottom-left
        };

        for (int i = 0; i < 6; i++) {
            m_cpuVertexData.insert(
                m_cpuVertexData.end(),
                &verts[i][0],
                &verts[i][6]
            );
        }
    }

    // GPU upload is omitted (requires staging buffer or host-visible memory).
    // This will be implemented later when UI batching is added.
}

} // namespace wave::render
