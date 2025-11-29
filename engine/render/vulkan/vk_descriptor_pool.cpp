#include "vk_descriptor_pool.hpp"

#include <utility> // std::exchange

namespace wave::engine::render::vulkan {

DescriptorPool::~DescriptorPool() {
    destroy();
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept {
    m_device = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_pool   = std::exchange(other.m_pool, VK_NULL_HANDLE);
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept {
    if (this != &other) {
        destroy();
        m_device = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_pool   = std::exchange(other.m_pool, VK_NULL_HANDLE);
    }
    return *this;
}

bool DescriptorPool::create(
    VkDevice device,
    std::uint32_t maxSets,
    const std::vector<VkDescriptorPoolSize>& poolSizes,
    VkDescriptorPoolCreateFlags flags
) {
    destroy();

    if (device == VK_NULL_HANDLE || maxSets == 0) {
        return false;
    }

    m_device = device;

    VkDescriptorPoolCreateInfo info{};
    info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.flags         = flags;
    info.maxSets       = maxSets;
    info.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    info.pPoolSizes    = poolSizes.empty() ? nullptr : poolSizes.data();

    if (vkCreateDescriptorPool(device, &info, nullptr, &m_pool) != VK_SUCCESS) {
        destroy();
        return false;
    }

    return true;
}

void DescriptorPool::destroy() {
    if (m_device != VK_NULL_HANDLE && m_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_pool, nullptr);
    }

    m_pool   = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
}

bool DescriptorPool::allocate_set(
    VkDescriptorSetLayout layout,
    VkDescriptorSet& outSet
) const {
    if (!m_device || !m_pool || layout == VK_NULL_HANDLE) {
        return false;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = m_pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts        = &layout;

    VkResult res = vkAllocateDescriptorSets(m_device, &allocInfo, &outSet);
    return res == VK_SUCCESS;
}

void DescriptorPool::reset() const {
    if (!m_device || !m_pool) {
        return;
    }

    vkResetDescriptorPool(m_device, m_pool, 0);
}

} // namespace wave::engine::render::vulkan
