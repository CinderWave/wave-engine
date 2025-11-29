#include "engine/render/vulkan/vk_pipeline.hpp"

#include "engine/core/logging/log.hpp"

namespace wave::engine::render::vulkan
{
    using wave::engine::core::logging::Logger;

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

    VkSimplePipeline::~VkSimplePipeline()
    {
        destroy();
    }

    VkSimplePipeline::VkSimplePipeline(VkSimplePipeline&& other) noexcept
    {
        m_device         = other.m_device;
        m_pipelineLayout = other.m_pipelineLayout;
        m_pipeline       = other.m_pipeline;
        m_vertShader     = std::move(other.m_vertShader);
        m_fragShader     = std::move(other.m_fragShader);

        other.m_device         = VK_NULL_HANDLE;
        other.m_pipelineLayout = VK_NULL_HANDLE;
        other.m_pipeline       = VK_NULL_HANDLE;
    }

    VkSimplePipeline&
    VkSimplePipeline::operator=(VkSimplePipeline&& other) noexcept
    {
        if (this != &other)
        {
            destroy();

            m_device         = other.m_device;
            m_pipelineLayout = other.m_pipelineLayout;
            m_pipeline       = other.m_pipeline;
            m_vertShader     = std::move(other.m_vertShader);
            m_fragShader     = std::move(other.m_fragShader);

            other.m_device         = VK_NULL_HANDLE;
            other.m_pipelineLayout = VK_NULL_HANDLE;
            other.m_pipeline       = VK_NULL_HANDLE;
        }

        return *this;
    }

    void VkSimplePipeline::destroy() noexcept
    {
        if (m_device != VK_NULL_HANDLE)
        {
            if (m_pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(m_device, m_pipeline, nullptr);
                m_pipeline = VK_NULL_HANDLE;
            }

            if (m_pipelineLayout != VK_NULL_HANDLE)
            {
                vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
                m_pipelineLayout = VK_NULL_HANDLE;
            }
        }

        m_vertShader  = VkShaderModuleHandle{};
        m_fragShader  = VkShaderModuleHandle{};
        m_device      = VK_NULL_HANDLE;
    }

    bool VkSimplePipeline::initialize(VkDevice device,
                                      VkRenderPass renderPass,
                                      VkExtent2D swapchainExtent,
                                      std::string_view vertResource,
                                      std::string_view fragResource)
    {
        destroy();

        if (device == VK_NULL_HANDLE || renderPass == VK_NULL_HANDLE)
        {
            WAVE_LOG_ERROR("[render] VkSimplePipeline::initialize called with null device or renderPass.");
            return false;
        }

        m_device = device;

        // Load shaders via resource system.
        if (!m_vertShader.load_from_resource(m_device, vertResource))
            return false;

        if (!m_fragShader.load_from_resource(m_device, fragResource))
            return false;

        VkPipelineShaderStageCreateInfo vertStage{};
        vertStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = m_vertShader.handle();
        vertStage.pName  = "main";

        VkPipelineShaderStageCreateInfo fragStage{};
        fragStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = m_fragShader.handle();
        fragStage.pName  = "main";

        VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

        // No vertex input (vertices generated in shader)
        VkPipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexBindingDescriptionCount   = 0;
        vertexInput.pVertexBindingDescriptions      = nullptr;
        vertexInput.vertexAttributeDescriptionCount = 0;
        vertexInput.pVertexAttributeDescriptions    = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(swapchainExtent.width);
        viewport.height   = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports    = &viewport;
        viewportState.scissorCount  = 1;
        viewportState.pScissors     = &scissor;

        VkPipelineRasterizationStateCreateInfo raster{};
        raster.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        raster.depthClampEnable        = VK_FALSE;
        raster.rasterizerDiscardEnable = VK_FALSE;
        raster.polygonMode             = VK_POLYGON_MODE_FILL;
        raster.cullMode                = VK_CULL_MODE_BACK_BIT;
        raster.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        raster.depthBiasEnable         = VK_FALSE;
        raster.lineWidth               = 1.0f;

        VkPipelineMultisampleStateCreateInfo multisample{};
        multisample.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.sampleShadingEnable  = VK_FALSE;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState blendAttachment{};
        blendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        blendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo blend{};
        blend.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blend.logicOpEnable   = VK_FALSE;
        blend.attachmentCount = 1;
        blend.pAttachments    = &blendAttachment;

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount         = 0;
        layoutInfo.pSetLayouts            = nullptr;
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges    = nullptr;

        if (!check_vk_result(vkCreatePipelineLayout(m_device, &layoutInfo, nullptr, &m_pipelineLayout),
                             "vkCreatePipelineLayout"))
        {
            return false;
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          = 2;
        pipelineInfo.pStages             = stages;
        pipelineInfo.pVertexInputState   = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &raster;
        pipelineInfo.pMultisampleState   = &multisample;
        pipelineInfo.pDepthStencilState  = nullptr;
        pipelineInfo.pColorBlendState    = &blend;
        pipelineInfo.pDynamicState       = nullptr;
        pipelineInfo.layout              = m_pipelineLayout;
        pipelineInfo.renderPass          = renderPass;
        pipelineInfo.subpass             = 0;
        pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex   = -1;

        VkResult res = vkCreateGraphicsPipelines(
            m_device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &m_pipeline
        );

        if (!check_vk_result(res, "vkCreateGraphicsPipelines"))
        {
            return false;
        }

        WAVE_LOG_INFO("[render] VkSimplePipeline initialized with shaders: ",
                      m_vertShader.debug_name(), " , ",
                      m_fragShader.debug_name());

        return true;
    }

    void VkSimplePipeline::shutdown() noexcept
    {
        destroy();
    }

} // namespace wave::engine::render::vulkan
