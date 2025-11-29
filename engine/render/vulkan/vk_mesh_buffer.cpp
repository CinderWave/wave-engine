#include "vk_mesh_buffer.hpp"

#include <utility>   // std::exchange
#include <cstring>   // std::memcpy

namespace wave::engine::render::vulkan {

namespace {

uint32_t find_memory_type(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties memProps{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        const bool suitable = (typeFilter & (1u << i)) != 0;
        const bool hasFlags = (memProps.memoryTypes[i].propertyFlags & properties) == properties;

        if (suitable && hasFlags) {
            return i;
        }
    }

    return 0u;
}

void create_buffer(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkBuffer& outBuffer,
    VkDeviceMemory& outMemory
) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size  = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(device, &bufferInfo, nullptr, &outBuffer);

    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(device, outBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = find_memory_type(
        physicalDevice,
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    vkAllocateMemory(device, &allocInfo, nullptr, &outMemory);
    vkBindBufferMemory(device, outBuffer, outMemory, 0);
}

void upload_data(
    VkDevice device,
    VkDeviceMemory memory,
    const void* data,
    VkDeviceSize size
) {
    if (!data || size == 0) {
        return;
    }

    void* mapped = nullptr;
    vkMapMemory(device, memory, 0, size, 0, &mapped);
    std::memcpy(mapped, data, static_cast<std::size_t>(size));
    vkUnmapMemory(device, memory);
}

} // namespace

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

MeshBuffer::~MeshBuffer() {
    destroy();
}

MeshBuffer::MeshBuffer(MeshBuffer&& other) noexcept {
    m_device       = std::exchange(other.m_device, VK_NULL_HANDLE);
    m_vertexBuffer = std::exchange(other.m_vertexBuffer, VK_NULL_HANDLE);
    m_vertexMemory = std::exchange(other.m_vertexMemory, VK_NULL_HANDLE);
    m_indexBuffer  = std::exchange(other.m_indexBuffer, VK_NULL_HANDLE);
    m_indexMemory  = std::exchange(other.m_indexMemory, VK_NULL_HANDLE);
    m_indexType    = std::exchange(other.m_indexType, VK_INDEX_TYPE_UINT32);
    m_indexCount   = std::exchange(other.m_indexCount, 0u);
}

MeshBuffer& MeshBuffer::operator=(MeshBuffer&& other) noexcept {
    if (this != &other) {
        destroy();

        m_device       = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_vertexBuffer = std::exchange(other.m_vertexBuffer, VK_NULL_HANDLE);
        m_vertexMemory = std::exchange(other.m_vertexMemory, VK_NULL_HANDLE);
        m_indexBuffer  = std::exchange(other.m_indexBuffer, VK_NULL_HANDLE);
        m_indexMemory  = std::exchange(other.m_indexMemory, VK_NULL_HANDLE);
        m_indexType    = std::exchange(other.m_indexType, VK_INDEX_TYPE_UINT32);
        m_indexCount   = std::exchange(other.m_indexCount, 0u);
    }
    return *this;
}

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

void MeshBuffer::create(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    const void* vertexData,
    std::uint32_t vertexCount,
    std::uint32_t vertexStride,
    const void* indexData,
    std::uint32_t indexCount,
    VkIndexType indexType
) {
    destroy();

    m_device     = device;
    m_indexType  = indexType;
    m_indexCount = indexCount;

    const VkDeviceSize vertexSizeBytes = static_cast<VkDeviceSize>(vertexCount) * vertexStride;
    const VkDeviceSize indexElementSize =
        (indexType == VK_INDEX_TYPE_UINT16) ? sizeof(std::uint16_t) : sizeof(std::uint32_t);
    const VkDeviceSize indexSizeBytes = static_cast<VkDeviceSize>(indexCount) * indexElementSize;

    // Vertex buffer
    create_buffer(
        physicalDevice,
        device,
        vertexSizeBytes,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        m_vertexBuffer,
        m_vertexMemory
    );
    upload_data(device, m_vertexMemory, vertexData, vertexSizeBytes);

    // Index buffer
    create_buffer(
        physicalDevice,
        device,
        indexSizeBytes,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        m_indexBuffer,
        m_indexMemory
    );
    upload_data(device, m_indexMemory, indexData, indexSizeBytes);
}

void MeshBuffer::destroy() {
    if (m_vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
        m_vertexBuffer = VK_NULL_HANDLE;
    }
    if (m_vertexMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_vertexMemory, nullptr);
        m_vertexMemory = VK_NULL_HANDLE;
    }

    if (m_indexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
        m_indexBuffer = VK_NULL_HANDLE;
    }
    if (m_indexMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_indexMemory, nullptr);
        m_indexMemory = VK_NULL_HANDLE;
    }

    m_device     = VK_NULL_HANDLE;
    m_indexType  = VK_INDEX_TYPE_UINT32;
    m_indexCount = 0u;
}

} // namespace wave::engine::render::vulkan
