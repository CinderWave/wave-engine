#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>

namespace wave::engine::render::vulkan {

class MeshBuffer final {
public:
    MeshBuffer() = default;
    ~MeshBuffer();

    MeshBuffer(const MeshBuffer&) = delete;
    MeshBuffer& operator=(const MeshBuffer&) = delete;

    MeshBuffer(MeshBuffer&& other) noexcept;
    MeshBuffer& operator=(MeshBuffer&& other) noexcept;

    // Create vertex and index buffers and upload data directly
    // Early version uses host visible memory for simplicity
    void create(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        const void* vertexData,
        std::uint32_t vertexCount,
        std::uint32_t vertexStride,
        const void* indexData,
        std::uint32_t indexCount,
        VkIndexType indexType
    );

    void destroy();

    VkBuffer     vertex_buffer() const { return m_vertexBuffer; }
    VkBuffer     index_buffer()  const { return m_indexBuffer;  }
    VkIndexType  index_type()    const { return m_indexType;    }
    std::uint32_t index_count()  const { return m_indexCount;   }

private:
    VkDevice      m_device{VK_NULL_HANDLE};

    VkBuffer      m_vertexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory m_vertexMemory{VK_NULL_HANDLE};

    VkBuffer      m_indexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory m_indexMemory{VK_NULL_HANDLE};

    VkIndexType    m_indexType{VK_INDEX_TYPE_UINT32};
    std::uint32_t  m_indexCount{0};
};

} // namespace wave::engine::render::vulkan
