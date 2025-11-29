#pragma once

#include "../../../editor/ui/render/ui_draw_list.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace wave::render {

class VulkanDevice;
class VulkanSwapchain;
class VulkanCommandPool;

class VulkanUIRenderer final {
public:
    VulkanUIRenderer() = default;
    ~VulkanUIRenderer();

    VulkanUIRenderer(const VulkanUIRenderer&) = delete;
    VulkanUIRenderer& operator=(const VulkanUIRenderer&) = delete;

    VulkanUIRenderer(VulkanUIRenderer&&) noexcept = default;
    VulkanUIRenderer& operator=(VulkanUIRenderer&&) noexcept = default;

    // Must be called after device + swapchain creation.
    bool initialize(VulkanDevice* device,
                    VulkanSwapchain* swapchain,
                    VulkanCommandPool* commandPool);

    void shutdown();

    // Called whenever swapchain is rebuilt.
    void recreate();

    // Build GPU work for this frame’s UI commands.
    // Called inside the editor render pass.
    void record_ui(VkCommandBuffer cmd,
                   const editor::ui::UIDrawList& drawList);

private:
    bool create_pipeline();
    void destroy_pipeline();

    bool create_vertex_buffer();
    void destroy_vertex_buffer();

    void upload_commands_to_buffer(const editor::ui::UIDrawList& drawList);

private:
    VulkanDevice*     m_device{nullptr};
    VulkanSwapchain*  m_swapchain{nullptr};
    VulkanCommandPool* m_commandPool{nullptr};

    VkPipelineLayout  m_pipelineLayout{VK_NULL_HANDLE};
    VkPipeline        m_pipeline{VK_NULL_HANDLE};

    VkBuffer          m_vertexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory    m_vertexMemory{VK_NULL_HANDLE};
    std::uint32_t     m_vertexStride{sizeof(float) * 6}; // pos(x,y), uv(x,y), color(r,g,b,a) simplified
    std::uint32_t     m_maxVertices{8192};

    std::vector<float> m_cpuVertexData;
};

} // namespace wave::render
