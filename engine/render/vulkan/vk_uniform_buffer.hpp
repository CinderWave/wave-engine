#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <cstddef>

namespace wave::engine::render::vulkan {

// Simple RAII wrapper around a host-visible uniform buffer.
// This is intended for small, frequently-updated data such as
// camera matrices, per-frame constants, etc.
class UniformBuffer final {
public:
    UniformBuffer() = default;
    ~UniformBuffer();

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

    UniformBuffer(UniformBuffer&& other) noexcept;
    UniformBuffer& operator=(UniformBuffer&& other) noexcept;

    // Create a uniform buffer of `size` bytes.
    //
    // Requirements:
    //  - device / physicalDevice must remain valid for the lifetime
    //    of this buffer.
    //  - size must be > 0 and respect the device's min uniform buffer
    //    alignment constraints (handled by caller).
    //
    // Memory properties:
    //  - HOST_VISIBLE | HOST_COHERENT so CPU can update frequently.
    //
    // Returns true on success, false on failure.
    bool create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkDeviceSize size
    );

    void destroy();

    bool valid() const {
        return m_buffer != VK_NULL_HANDLE;
    }

    VkBuffer buffer() const { return m_buffer; }
    VkDeviceSize size() const { return m_size; }

    // Map the buffer's memory and return a CPU pointer.
    // It is safe to call update() instead of using map/unmap directly.
    void* map();
    void  unmap();

    // Convenience: copy `size` bytes from `data` into the buffer.
    // This calls map/unmap internally.
    void update(const void* data, VkDeviceSize size);

private:
    static std::uint32_t find_memory_type(
        VkPhysicalDevice physicalDevice,
        std::uint32_t typeFilter,
        VkMemoryPropertyFlags properties
    );

private:
    VkDevice        m_device{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

    VkBuffer        m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory  m_memory{VK_NULL_HANDLE};
    VkDeviceSize    m_size{0};

    void*           m_mapped{nullptr};
};

} // namespace wave::engine::render::vulkan
