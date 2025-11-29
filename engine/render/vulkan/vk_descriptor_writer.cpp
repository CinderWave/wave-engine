#include "vk_descriptor_writer.hpp"

namespace wave::engine::render::vulkan {

DescriptorWriter& DescriptorWriter::write_uniform_buffer(
    VkDescriptorSet dstSet,
    std::uint32_t binding,
    const VkDescriptorBufferInfo* bufferInfo,
    VkDescriptorType type
) {
    if (!bufferInfo) {
        return *this;
    }

    m_bufferInfos.push_back(*bufferInfo);
    VkDescriptorBufferInfo* storedInfo = &m_bufferInfos.back();

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet          = dstSet;
    write.dstBinding      = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType  = type;
    write.pBufferInfo     = storedInfo;
    write.pImageInfo      = nullptr;
    write.pTexelBufferView = nullptr;

    m_writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::write_image_sampler(
    VkDescriptorSet dstSet,
    std::uint32_t binding,
    const VkDescriptorImageInfo* imageInfo,
    VkDescriptorType type
) {
    if (!imageInfo) {
        return *this;
    }

    m_imageInfos.push_back(*imageInfo);
    VkDescriptorImageInfo* storedInfo = &m_imageInfos.back();

    VkWriteDescriptorSet write{};
    write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet          = dstSet;
    write.dstBinding      = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType  = type;
    write.pBufferInfo     = nullptr;
    write.pImageInfo      = storedInfo;
    write.pTexelBufferView = nullptr;

    m_writes.push_back(write);
    return *this;
}

void DescriptorWriter::commit(VkDevice device) {
    if (!device || m_writes.empty()) {
        clear();
        return;
    }

    vkUpdateDescriptorSets(
        device,
        static_cast<std::uint32_t>(m_writes.size()), m_writes.data(),
        0, nullptr
    );

    clear();
}

void DescriptorWriter::clear() {
    m_writes.clear();
    m_bufferInfos.clear();
    m_imageInfos.clear();
}

} // namespace wave::engine::render::vulkan
