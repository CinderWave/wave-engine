#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace wave::engine::render::vulkan {

class DescriptorSetLayout final {
public:
    DescriptorSetLayout() = default;
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

    void create(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    VkDescriptorSetLayout handle() const { return m_layout; }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkDescriptorSetLayout m_layout{VK_NULL_HANDLE};
};

class DescriptorPool final {
public:
    DescriptorPool() = default;
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(DescriptorPool&& other) noexcept;

    void create(VkDevice device, const std::vector<VkDescriptorPoolSize>& sizes, uint32_t maxSets);

    VkDescriptorPool handle() const { return m_pool; }

private:
    VkDevice m_device{VK_NULL_HANDLE};
    VkDescriptorPool m_pool{VK_NULL_HANDLE};
};

class DescriptorSet final {
public:
    DescriptorSet() = default;

    void allocate(
        VkDevice device,
        VkDescriptorPool pool,
        VkDescriptorSetLayout layout
    );

    VkDescriptorSet handle() const { return m_set; }

private:
    VkDescriptorSet m_set{VK_NULL_HANDLE};
};

} // namespace wave::engine::render::vulkan
