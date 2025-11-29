#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>

#include "vk_uniform_buffer.hpp"
#include "vk_descriptor_set_layout.hpp"
#include "vk_descriptor_pool.hpp"
#include "vk_descriptor_writer.hpp"

namespace wave::engine::render::vulkan {

// Simple global per-frame uniform data.
// This is POD and matches std140-friendly layout.
// A future camera system will fill view/proj matrices.
struct GlobalFrameUBO {
    // Column-major 4x4 matrices as raw floats.
    float view[16];
    float proj[16];
    float viewProj[16];

    float time;       // seconds since start
    float deltaTime;  // seconds since last frame
    float padding0[2]; // pad to 16-byte multiple
};

// Manages per-frame uniform buffers and descriptor sets
// for a single global UBO binding (binding 0).
class FrameUniforms final {
public:
    FrameUniforms() = default;
    ~FrameUniforms();

    FrameUniforms(const FrameUniforms&) = delete;
    FrameUniforms& operator=(const FrameUniforms&) = delete;

    FrameUniforms(FrameUniforms&&) noexcept;
    FrameUniforms& operator=(FrameUniforms&&) noexcept;

    // Initialize per-frame UBO + descriptors.
    //
    // - device / physicalDevice: Vulkan handles
    // - framesInFlight: number of swapchain images or frames in flight
    // - shaderStageFlags: which shader stages see this UBO
    //
    // On success, you get:
    //  - a descriptor set layout with binding 0 = uniform buffer
    //  - a descriptor pool
    //  - one UniformBuffer + descriptor set per frame
    bool initialize(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        std::uint32_t framesInFlight,
        VkShaderStageFlags shaderStageFlags
    );

    void shutdown();

    bool valid() const { return m_device != VK_NULL_HANDLE && !m_buffers.empty(); }

    std::uint32_t frames_in_flight() const { return static_cast<std::uint32_t>(m_buffers.size()); }

    // Update the UBO for a given frame index.
    void update_frame_data(std::uint32_t frameIndex, const GlobalFrameUBO& data);

    // Get descriptor set for a given frame index.
    VkDescriptorSet descriptor_set(std::uint32_t frameIndex) const;

    // Get descriptor set layout for pipeline creation.
    VkDescriptorSetLayout layout() const { return m_layout.handle(); }

private:
    void destroy_resources();

private:
    VkDevice         m_device{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

    DescriptorSetLayout m_layout;
    DescriptorPool      m_pool;

    std::vector<UniformBuffer> m_buffers;
    std::vector<VkDescriptorSet> m_descriptorSets;
};

} // namespace wave::engine::render::vulkan
