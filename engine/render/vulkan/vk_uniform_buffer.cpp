#include "vk_uniform_buffer.hpp"

#include <cstring>  // std::memcpy
#include <utility>  // std::exchange

namespace wave::engine::render::vulkan {

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

UniformBuffer::~UniformBuffer() {
    destroy();
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept {
    m_device         = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_physicalDevice = std::exchange(other.m_physicalDevice, VK_NULL_HANDLE);
    m_buffer         = std::exchange(other.m_buffer, VK_NULL_HANDLE);
    m_memory         = std::exchange(other.m_memory, VK_NULL_HANDLE);
    m_size           = std::exchange(other.m_size, 0);
    m_mapped         = std::exchange(other.m_mapped, nullptr);
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept {
    if (this != &other) {
        destroy();

        m_device         = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_physicalDevice = std::exchange(other.m_physicalDevice, VK_NULL_HANDLE);
        m_buffer         = std::exchange(other.m_buffer, VK_NULL_HANDLE);
        m_memory         = std::exchange(other.m_memory, VK_NULL_HANDLE);
        m_size           = std::exchange(other.m_size, 0);
        m_mapped         = std::exchange(other.m_mapped, nullptr);
    }
    return *this;
}

// -----------------------------------------------------------------------------
// Creation / destruction
// -----------------------------------------------------------------------------

bool UniformBuffer::create(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDeviceSize size
) {
    destroy();

    if (device == VK_NULL_HANDLE || physicalDevice == VK_NULL_HANDLE || size == 0) {
        return false;
    }

    m_device         = device;
    m_physicalDevice = physicalDevice;
    m_size           = size;

    // Create buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        destroy();
        return false;
    }

    // Allocate memory
    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(device, m_buffer, &memReq);

    VkMemoryPropertyFlags properties =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    const std::uint32_t memoryTypeIndex =
        find_memory_type(physicalDevice, memReq.memoryTypeBits, properties);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        destroy();
        return false;
    }

    vkBindBufferMemory(device, m_buffer, m_memory, 0);

    return true;
}

void UniformBuffer::destroy() {
    if (m_device != VK_NULL_HANDLE) {
        if (m_mapped) {
            vkUnmapMemory(m_device, m_memory);
            m_mapped = nullptr;
        }

        if (m_buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_device, m_buffer, nullptr);
            m_buffer = VK_NULL_HANDLE;
        }

        if (m_memory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_memory, nullptr);
            m_memory = VK_NULL_HANDLE;
        }
    }

    m_device         = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
    m_size           = 0;
    m_mapped         = nullptr;
}

// -----------------------------------------------------------------------------
// Mapping / updating
// -----------------------------------------------------------------------------

void* UniformBuffer::map() {
    if (!m_device || !m_memory) {
        return nullptr;
    }

    if (!m_mapped) {
        vkMapMemory(m_device, m_memory, 0, m_size, 0, &m_mapped);
    }

    return m_mapped;
}

void UniformBuffer::unmap() {
    if (m_mapped && m_device && m_memory) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }
}

void UniformBuffer::update(const void* data, VkDeviceSize size) {
    if (!data || size == 0 || size > m_size) {
        return;
    }

    void* dst = map();
    if (!dst) {
        return;
    }

    std::memcpy(dst, data, static_cast<std::size_t>(size));
    // Memory is HOST_COHERENT, so no explicit flush required.
    // If you switch to non-coherent memory, add vkFlushMappedMemoryRanges here.
}

// -----------------------------------------------------------------------------
// Memory type helper
// -----------------------------------------------------------------------------

std::uint32_t UniformBuffer::find_memory_type(
    VkPhysicalDevice physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

    for (std::uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        const bool suitable = (typeFilter & (1u << i)) != 0;
        const bool hasProps =
            (memProps.memoryTypes[i].propertyFlags & properties) == properties;

        if (suitable && hasProps) {
            return i;
        }
    }

    // Fallback: 0. In practice, callers should ensure a valid type exists.
    return 0;
}

} // namespace wave::engine::render::vulkan
