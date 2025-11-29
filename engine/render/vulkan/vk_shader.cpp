#include "engine/render/vulkan/vk_shader.hpp"

#include "engine/core/logging/log.hpp"
#include "engine/core/resources/resource_system.hpp"

namespace wave::engine::render::vulkan
{
    using wave::engine::core::logging::Logger;
    using wave::engine::core::resources::ResourceSystem;

    namespace
    {
        bool check_vk_result(VkResult result, const char* context)
        {
            if (result == VK_SUCCESS)
                return true;

            WAVE_LOG_ERROR("[render] Vulkan error in ", context,
                           " (code = ", static_cast<int>(result), ")");
            return false;
        }
    }

    VkShaderModuleHandle::~VkShaderModuleHandle()
    {
        destroy();
    }

    VkShaderModuleHandle::VkShaderModuleHandle(VkShaderModuleHandle&& other) noexcept
    {
        m_device    = other.m_device;
        m_module    = other.m_module;
        m_debugName = std::move(other.m_debugName);

        other.m_device = VK_NULL_HANDLE;
        other.m_module = VK_NULL_HANDLE;
    }

    VkShaderModuleHandle&
    VkShaderModuleHandle::operator=(VkShaderModuleHandle&& other) noexcept
    {
        if (this != &other)
        {
            destroy();

            m_device    = other.m_device;
            m_module    = other.m_module;
            m_debugName = std::move(other.m_debugName);

            other.m_device = VK_NULL_HANDLE;
            other.m_module = VK_NULL_HANDLE;
        }

        return *this;
    }

    void VkShaderModuleHandle::destroy() noexcept
    {
        if (m_module != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device, m_module, nullptr);
        }

        m_module = VK_NULL_HANDLE;
        m_device = VK_NULL_HANDLE;
    }

    bool VkShaderModuleHandle::load_from_resource(VkDevice device,
                                                  std::string_view resourcePath)
    {
        destroy();

        if (device == VK_NULL_HANDLE)
        {
            WAVE_LOG_ERROR("[render] VkShaderModuleHandle::load_from_resource called with null device.");
            return false;
        }

        std::vector<std::uint8_t> data;
        if (!ResourceSystem::load_binary(resourcePath, data))
        {
            WAVE_LOG_ERROR("[render] Failed to load shader resource: ", std::string(resourcePath));
            return false;
        }

        if (data.empty() || (data.size() % 4) != 0)
        {
            WAVE_LOG_ERROR("[render] Shader resource not valid SPIR-V size: ", std::string(resourcePath));
            return false;
        }

        VkShaderModuleCreateInfo info{};
        info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = data.size();
        info.pCode    = reinterpret_cast<const std::uint32_t*>(data.data());

        VkShaderModule module = VK_NULL_HANDLE;
        if (!check_vk_result(vkCreateShaderModule(device, &info, nullptr, &module),
                             "vkCreateShaderModule"))
        {
            return false;
        }

        m_device    = device;
        m_module    = module;
        m_debugName = std::string(resourcePath);

        WAVE_LOG_INFO("[render] Shader module created from resource: ", m_debugName);

        return true;
    }

} // namespace wave::engine::render::vulkan
