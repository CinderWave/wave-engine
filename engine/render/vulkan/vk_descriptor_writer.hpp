#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace wave::engine::render::vulkan {

// Lightweight helper to build vkUpdateDescriptorSets() calls.
// Not RAII, just a convenience for writing uniform / image descriptors.
class DescriptorWriter final {
public:
    DescriptorWriter() = default;
    ~DescriptorWriter() = default;

    DescriptorWriter(const DescriptorWriter&) = delete;
    DescriptorWriter& operator=(const DescriptorWriter&) = delete;

    DescriptorWriter(DescriptorWriter&&) noexcept = default;
    DescriptorWriter& operator=(DescriptorWriter&&) noexcept = default;

    // Add a uniform buffer binding to be written into `dstSet`.
    DescriptorWriter& write_uniform_buffer(
        VkDescriptorSet       dstSet,
        std::uint32_t         binding,
        const VkDescriptorBufferInfo* bufferInfo,
        VkDescriptorType      type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );

    // Add a combined image sampler binding to be written into `dstSet`.
    DescriptorWriter& write_image_sampler(
        VkDescriptorSet       dstSet,
        std::uint32_t         binding,
        const VkDescriptorImageInfo* imageInfo,
        VkDescriptorType      type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    );

    // Execute vkUpdateDescriptorSets on the given device with all queued writes.
    void commit(VkDevice device);

    // Clear queued writes without committing.
    void clear();

private:
    std::vector<VkWriteDescriptorSet> m_writes;
    std::vector<VkDescriptorBufferInfo> m_bufferInfos;
    std::vector<VkDescriptorImageInfo>  m_imageInfos;
};

} // namespace wave::engine::render::vulkan
