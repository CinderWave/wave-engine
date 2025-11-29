#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace wave::engine::render::vulkan {

// RAII wrapper around VkDescriptorSetLayout.
class DescriptorSetLayout final {
public:
    DescriptorSetLayout() = default;
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

    // Create a descriptor set layout with the provided bindings.
    // `bindings` must remain valid only for the duration of this call.
    bool create(
        VkDevice device,
        const std::vector<VkDescriptorSetLayoutBinding>& bindings
    );

    void destroy();

    VkDescriptorSetLayout handle() const { return m_layout; }
    bool valid() const { return m_layout != VK_NULL_HANDLE; }

private:
    VkDevice              m_device{VK_NULL_HANDLE};
    VkDescriptorSetLayout m_layout{VK_NULL_HANDLE};
};

} // namespace wave::engine::render::vulkan
