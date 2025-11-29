#include "vk_descriptor_set_layout.hpp"

#include <utility> // std::exchange

namespace wave::engine::render::vulkan {

DescriptorSetLayout::~DescriptorSetLayout() {
    destroy();
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept {
    m_device = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_layout = std::exchange(other.m_layout, VK_NULL_HANDLE);
}

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept {
    if (this != &other) {
        destroy();
        m_device = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_layout = std::exchange(other.m_layout, VK_NULL_HANDLE);
    }
    return *this;
}

bool DescriptorSetLayout::create(
    VkDevice device,
    const std::vector<VkDescriptorSetLayoutBinding>& bindings
) {
    destroy();

    if (device == VK_NULL_HANDLE) {
        return false;
    }

    m_device = device;

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = static_cast<std::uint32_t>(bindings.size());
    info.pBindings    = bindings.empty() ? nullptr : bindings.data();

    if (vkCreateDescriptorSetLayout(device, &info, nullptr, &m_layout) != VK_SUCCESS) {
        destroy();
        return false;
    }

    return true;
}

void DescriptorSetLayout::destroy() {
    if (m_device != VK_NULL_HANDLE && m_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
    }

    m_layout = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
}

} // namespace wave::engine::render::vulkan
