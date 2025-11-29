#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace wave::engine::render::vulkan {

// RAII wrapper for a depth image + view.
// This does not own the device or physical device lifelong.
// Typical usage:
//   - choose_depth_format()
//   - create(...) after swapchain creation
//   - recreate(...) on swapchain resize
//   - destroy() on shutdown
class DepthBuffer final {
public:
    DepthBuffer() = default;
    ~DepthBuffer();

    DepthBuffer(const DepthBuffer&) = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;

    DepthBuffer(DepthBuffer&& other) noexcept;
    DepthBuffer& operator=(DepthBuffer&& other) noexcept;

    // Create a depth image and view with given format and extent.
    //
    // Requirements:
    //   - device and physicalDevice are valid
    //   - extent.width and extent.height are nonzero
    //
    // The image uses:
    //   - Tiling: optimal
    //   - Usage: depth stencil attachment (and optional sampled)
    bool create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D extent,
        VkFormat format,
        VkImageUsageFlags extraUsageFlags = 0
    );

    void destroy();

    bool valid() const { return m_image != VK_NULL_HANDLE; }

    VkImage        image() const { return m_image; }
    VkImageView    view()  const { return m_view; }
    VkFormat       format() const { return m_format; }
    VkExtent2D     extent() const { return m_extent; }

    // Helper for swapchain resize:
    // Destroys the old image and re-creates it with the same format.
    bool recreate(VkExtent2D newExtent);

    // Utility to select a supported depth format on this GPU.
    // It tries common depth formats in order of preference.
    static VkFormat choose_depth_format(
        VkPhysicalDevice physicalDevice
    );

private:
    static std::uint32_t find_memory_type(
        VkPhysicalDevice physicalDevice,
        std::uint32_t typeFilter,
        VkMemoryPropertyFlags properties
    );

    void move_from(DepthBuffer& other) noexcept;

private:
    VkDevice         m_device{VK_NULL_HANDLE};
    VkPhysicalDevice m_physicalDevice{VK_NULL_HANDLE};

    VkImage          m_image{VK_NULL_HANDLE};
    VkDeviceMemory   m_memory{VK_NULL_HANDLE};
    VkImageView      m_view{VK_NULL_HANDLE};

    VkFormat         m_format{VK_FORMAT_UNDEFINED};
    VkExtent2D       m_extent{0u, 0u};
};

} // namespace wave::engine::render::vulkan
