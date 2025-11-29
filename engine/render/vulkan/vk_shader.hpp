#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

namespace wave::engine::render::vulkan
{
    // Lightweight RAII wrapper for a VkShaderModule.
    //
    // Responsibilities:
    //  - Load SPIR-V binary via the core ResourceSystem
    //  - Create a VkShaderModule from it
    //  - Destroy the module when going out of scope
    //
    // Usage:
    //   VkShaderModuleHandle vert;
    //   if (vert.load_from_resource(device, "shaders/editor/basic.vert.spv"))
    //   {
    //       VkPipelineShaderStageCreateInfo stage{};
    //       stage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    //       stage.module = vert.handle();
    //       ...
    //   }
    //
    class VkShaderModuleHandle
    {
    public:
        VkShaderModuleHandle() = default;
        ~VkShaderModuleHandle();

        VkShaderModuleHandle(const VkShaderModuleHandle&)            = delete;
        VkShaderModuleHandle& operator=(const VkShaderModuleHandle&) = delete;

        VkShaderModuleHandle(VkShaderModuleHandle&& other) noexcept;
        VkShaderModuleHandle& operator=(VkShaderModuleHandle&& other) noexcept;

        // Load SPIR-V from a resource-relative path, e.g.:
        //   "shaders/editor/clear_color.vert.spv"
        //
        // Returns true on success, false on failure.
        bool load_from_resource(VkDevice device, std::string_view resourcePath);

        // Is a valid shader module currently loaded.
        bool is_valid() const noexcept { return m_module != VK_NULL_HANDLE; }

        // Underlying VkShaderModule.
        VkShaderModule handle() const noexcept { return m_module; }

        // For debugging / logging.
        const std::string& debug_name() const noexcept { return m_debugName; }

    private:
        void destroy() noexcept;

        VkDevice       m_device    = VK_NULL_HANDLE;
        VkShaderModule m_module    = VK_NULL_HANDLE;
        std::string    m_debugName;
    };

} // namespace wave::engine::render::vulkan
