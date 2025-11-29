#include "vk_depth_buffer.hpp"

#include <utility>

namespace wave::engine::render::vulkan {

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

DepthBuffer::~DepthBuffer() {
    destroy();
}

DepthBuffer::DepthBuffer(DepthBuffer&& other) noexcept {
    move_from(other);
}

DepthBuffer& DepthBuffer::operator=(DepthBuffer&& other) noexcept {
    if (this != &other) {
        destroy();
        move_from(other);
    }
    return *this;
}

void DepthBuffer::move_from(DepthBuffer& other) noexcept {
    m_device         = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_physicalDevice = std::exchange(other.m_physicalDevice, VK_NULL_HANDLE);

    m_image   = std::exchange(other.m_image, VK_NULL_HANDLE);
    m_memory  = std::exchange(other.m_memory, VK_NULL_HANDLE);
    m_view    = std::exchange(other.m_view, VK_NULL_HANDLE);
    m_format  = other.m_format;
    m_extent  = other.m_extent;
}

// -----------------------------------------------------------------------------
// Creation / destruction
// -----------------------------------------------------------------------------

bool DepthBuffer::create(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkExtent2D extent,
    VkFormat format,
    VkImageUsageFlags extraUsageFlags
) {
    destroy();

    if (device == VK_NULL_HANDLE ||
        physicalDevice == VK_NULL_HANDLE ||
        extent.width == 0 ||
        extent.height == 0 ||
        format == VK_FORMAT_UNDEFINED) {
        return false;
    }

    m_device         = device;
    m_physicalDevice = physicalDevice;
    m_extent         = extent;
    m_format         = format;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.format        = format;
    imageInfo.extent.width  = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                              extraUsageFlags;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(device, &imageInfo, nullptr, &m_image) != VK_SUCCESS) {
        destroy();
        return false;
    }

    VkMemoryRequirements memReq{};
    vkGetImageMemoryRequirements(device, m_image, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = find_memory_type(
        physicalDevice,
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        destroy();
        return false;
    }

    vkBindImageMemory(device, m_image, m_memory, 0);

    // Create image view
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (m_format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
        m_format == VK_FORMAT_D24_UNORM_S8_UINT) {
        aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = m_image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = m_format;
    viewInfo.subresourceRange.aspectMask     = aspectMask;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr, &m_view) != VK_SUCCESS) {
        destroy();
        return false;
    }

    return true;
}

void DepthBuffer::destroy() {
    if (m_device != VK_NULL_HANDLE) {
        if (m_view != VK_NULL_HANDLE) {
            vkDestroyImageView(m_device, m_view, nullptr);
        }
        if (m_image != VK_NULL_HANDLE) {
            vkDestroyImage(m_device, m_image, nullptr);
        }
        if (m_memory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_memory, nullptr);
        }
    }

    m_view    = VK_NULL_HANDLE;
    m_image   = VK_NULL_HANDLE;
    m_memory  = VK_NULL_HANDLE;
    m_device  = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    m_format  = VK_FORMAT_UNDEFINED;
    m_extent  = {0u, 0u};
}

bool DepthBuffer::recreate(VkExtent2D newExtent) {
    if (m_device == VK_NULL_HANDLE ||
        m_physicalDevice == VK_NULL_HANDLE ||
        m_format == VK_FORMAT_UNDEFINED) {
        return false;
    }

    return create(m_device, m_physicalDevice, newExtent, m_format);
}

// -----------------------------------------------------------------------------
// Format selection
// -----------------------------------------------------------------------------

VkFormat DepthBuffer::choose_depth_format(
    VkPhysicalDevice physicalDevice
) {
    const VkFormat candidates[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props{};
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    // Fallback
    return VK_FORMAT_D32_SFLOAT;
}

// -----------------------------------------------------------------------------
// Memory type helper
// -----------------------------------------------------------------------------

std::uint32_t DepthBuffer::find_memory_type(
    VkPhysicalDevice physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

    for (std::uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        bool suitable = (typeFilter & (1u << i)) != 0;
        bool hasProps =
            (memProps.memoryTypes[i].propertyFlags & properties) == properties;

        if (suitable && hasProps) {
            return i;
        }
    }

    // Fallback, should not happen on a sane device
    return 0;
}

} // namespace wave::engine::render::vulkan
