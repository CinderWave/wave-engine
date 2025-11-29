#include "vk_frame_uniforms.hpp"

#include <algorithm>

namespace wave::engine::render::vulkan {

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

FrameUniforms::~FrameUniforms() {
    shutdown();
}

FrameUniforms::FrameUniforms(FrameUniforms&& other) noexcept {
    m_device         = other.m_device;
    m_physicalDevice = other.m_physicalDevice;

    m_layout         = std::move(other.m_layout);
    m_pool           = std::move(other.m_pool);
    m_buffers        = std::move(other.m_buffers);
    m_descriptorSets = std::move(other.m_descriptorSets);

    other.m_device         = VK_NULL_HANDLE;
    other.m_physicalDevice = VK_NULL_HANDLE;
}

FrameUniforms& FrameUniforms::operator=(FrameUniforms&& other) noexcept {
    if (this != &other) {
        shutdown();

        m_device         = other.m_device;
        m_physicalDevice = other.m_physicalDevice;

        m_layout         = std::move(other.m_layout);
        m_pool           = std::move(other.m_pool);
        m_buffers        = std::move(other.m_buffers);
        m_descriptorSets = std::move(other.m_descriptorSets);

        other.m_device         = VK_NULL_HANDLE;
        other.m_physicalDevice = VK_NULL_HANDLE;
    }
    return *this;
}

// -----------------------------------------------------------------------------
// Init / shutdown
// -----------------------------------------------------------------------------

bool FrameUniforms::initialize(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    std::uint32_t framesInFlight,
    VkShaderStageFlags shaderStageFlags
) {
    shutdown();

    if (device == VK_NULL_HANDLE ||
        physicalDevice == VK_NULL_HANDLE ||
        framesInFlight == 0) {
        return false;
    }

    m_device         = device;
    m_physicalDevice = physicalDevice;

    // 1) Descriptor set layout: binding 0 = uniform buffer
    VkDescriptorSetLayoutBinding binding{};
    binding.binding         = 0;
    binding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags      = shaderStageFlags;
    binding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> bindings{binding};
    if (!m_layout.create(device, bindings)) {
        shutdown();
        return false;
    }

    // 2) Descriptor pool with enough uniform buffers for all frames
    VkDescriptorPoolSize poolSize{};
    poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = framesInFlight;

    std::vector<VkDescriptorPoolSize> poolSizes{poolSize};

    if (!m_pool.create(device, framesInFlight, poolSizes)) {
        shutdown();
        return false;
    }

    // 3) Create per-frame uniform buffers
    m_buffers.resize(framesInFlight);
    const VkDeviceSize bufSize = sizeof(GlobalFrameUBO);

    for (std::uint32_t i = 0; i < framesInFlight; ++i) {
        if (!m_buffers[i].create(device, physicalDevice, bufSize)) {
            shutdown();
            return false;
        }
    }

    // 4) Allocate descriptor sets
    m_descriptorSets.resize(framesInFlight);

    for (std::uint32_t i = 0; i < framesInFlight; ++i) {
        if (!m_pool.allocate_set(m_layout.handle(), m_descriptorSets[i])) {
            shutdown();
            return false;
        }
    }

    // 5) Write descriptors
    DescriptorWriter writer;

    for (std::uint32_t i = 0; i < framesInFlight; ++i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_buffers[i].buffer();
        bufferInfo.offset = 0;
        bufferInfo.range  = bufSize;

        writer.write_uniform_buffer(
            m_descriptorSets[i],
            0,
            &bufferInfo,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        );
    }

    writer.commit(device);

    return true;
}

void FrameUniforms::shutdown() {
    destroy_resources();

    m_device         = VK_NULL_HANDLE;
    m_physicalDevice = VK_NULL_HANDLE;
}

void FrameUniforms::destroy_resources() {
    m_descriptorSets.clear(); // Vulkan frees them when pool is destroyed.

    for (auto& ub : m_buffers) {
        ub.destroy();
    }
    m_buffers.clear();

    m_pool.destroy();
    m_layout.destroy();
}

// -----------------------------------------------------------------------------
// Per-frame operations
// -----------------------------------------------------------------------------

void FrameUniforms::update_frame_data(std::uint32_t frameIndex, const GlobalFrameUBO& data) {
    if (frameIndex >= m_buffers.size()) {
        return;
    }

    m_buffers[frameIndex].update(&data, sizeof(GlobalFrameUBO));
}

VkDescriptorSet FrameUniforms::descriptor_set(std::uint32_t frameIndex) const {
    if (frameIndex >= m_descriptorSets.size()) {
        return VK_NULL_HANDLE;
    }
    return m_descriptorSets[frameIndex];
}

} // namespace wave::engine::render::vulkan
