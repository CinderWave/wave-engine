#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

namespace wave::engine::render::vulkan {

// RAII wrapper around VkDescriptorPool with a simple allocator API.
// Designed for per-frame or per-pipeline descriptor allocation.
class DescriptorPool final {
public:
    DescriptorPool() = default;
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(DescriptorPool&& other) noexcept;

    // Create a descriptor pool.
    //
    // - maxSets: approximate max number of descriptor sets to allocate.
    // - poolSizes: descriptor counts per type.
    bool create(
        VkDevice device,
        std::uint32_t maxSets,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        VkDescriptorPoolCreateFlags flags = 0
    );

    void destroy();

    bool valid() const { return m_pool != VK_NULL_HANDLE; }
    VkDescriptorPool handle() const { return m_pool; }

    // Allocate a single descriptor set from the pool.
    bool allocate_set(
        VkDescriptorSetLayout layout,
        VkDescriptorSet& outSet
    ) const;

    // Reset the entire pool, freeing all descriptor sets.
    void reset() const;

private:
    VkDevice        m_device{VK_NULL_HANDLE};
    VkDescriptorPool m_pool{VK_NULL_HANDLE};
};

} // namespace wave::engine::render::vulkan
