#include "vk_descriptor.hpp"
#include <utility>

using namespace wave::engine::render::vulkan;

// DescriptorSetLayout -------------------------------------------------------

DescriptorSetLayout::~DescriptorSetLayout() {
    if (m_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept {
    m_device = other.m_device;
    m_layout = other.m_layout;
    other.m_layout = VK_NULL_HANDLE;
}

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept {
    if (this != &other) {
        if (m_layout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
        }

        m_device = other.m_device;
        m_layout = other.m_layout;
        other.m_layout = VK_NULL_HANDLE;
    }
    return *this;
}

void DescriptorSetLayout::create(
    VkDevice device,
    const std::vector<VkDescriptorSetLayoutBinding>& bindings
) {
    m_device = device;

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = static_cast<uint32_t>(bindings.size());
    info.pBindings = bindings.data();

    vkCreateDescriptorSetLayout(device, &info, nullptr, &m_layout);
}

// DescriptorPool -------------------------------------------------------------

DescriptorPool::~DescriptorPool() {
    if (m_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept {
    m_device = other.m_device;
    m_pool = other.m_pool;
    other.m_pool = VK_NULL_HANDLE;
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept {
    if (this != &other) {
        if (m_pool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m_device, m_pool, nullptr);
        }

        m_device = other.m_device;
        m_pool = other.m_pool;
        other.m_pool = VK_NULL_HANDLE;
    }
    return *this;
}

void DescriptorPool::create(
    VkDevice device,
    const std::vector<VkDescriptorPoolSize>& sizes,
    uint32_t maxSets
) {
    m_device = device;

    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.poolSizeCount = static_cast<uint32_t>(sizes.size());
    info.pPoolSizes = sizes.data();
    info.maxSets = maxSets;

    vkCreateDescriptorPool(device, &info, nullptr, &m_pool);
}

// DescriptorSet --------------------------------------------------------------

void DescriptorSet::allocate(
    VkDevice device,
    VkDescriptorPool pool,
    VkDescriptorSetLayout layout
) {
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = pool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;

    vkAllocateDescriptorSets(device, &info, &m_set);
}
