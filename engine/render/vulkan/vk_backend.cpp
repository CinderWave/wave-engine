#include "engine/render/vulkan/vk_backend.hpp"

#include "engine/core/logging/log.hpp"

#include <stdexcept>
#include <vector>
#include <set>
#include <optional>
#include <limits>

#include <GLFW/glfw3.h> // glfwGetRequiredInstanceExtensions, glfwCreateWindowSurface

namespace wave::engine::render::vulkan
{
    using wave::engine::core::logging::Logger;

    namespace
    {
        constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    }

    static bool check_vk_result(VkResult result, const char* context)
    {
        if (result == VK_SUCCESS)
            return true;

        WAVE_LOG_ERROR("[render] Vulkan error in ", context,
                       " (code = ", static_cast<int>(result), ")");
        return false;
    }

    static const std::vector<const char*> kRequiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkBackend::~VkBackend()
    {
        shutdown();
    }

    bool VkBackend::initialize(const RenderInitInfo& info)
    {
        m_window_handle = info.window_handle;
        m_width         = info.width;
        m_height        = info.height;

        m_debug_name = "VkBackend";

        if (!m_window_handle)
        {
            WAVE_LOG_ERROR("[render] VkBackend::initialize called with null window handle.");
            return false;
        }

        auto* glfwWindow = static_cast<GLFWwindow*>(m_window_handle);

        WAVE_LOG_INFO("[render] Initializing Vulkan backend...");
        WAVE_LOG_INFO("[render] Window: ", m_width, "x", m_height);

        // 1) Instance

        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "WaveEditor";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName        = "Wave Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_2;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        if (!glfwExtensions || glfwExtensionCount == 0)
        {
            WAVE_LOG_ERROR("[render] glfwGetRequiredInstanceExtensions returned no extensions.");
            return false;
        }

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo        = &appInfo;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount       = 0;
        createInfo.ppEnabledLayerNames     = nullptr;

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
        if (!check_vk_result(result, "vkCreateInstance"))
        {
            m_instance = VK_NULL_HANDLE;
            return false;
        }

        WAVE_LOG_INFO("[render] Vulkan instance created.");

        // 2) Surface

        result = glfwCreateWindowSurface(m_instance, glfwWindow, nullptr, &m_surface);
        if (!check_vk_result(result, "glfwCreateWindowSurface"))
        {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
            m_surface  = VK_NULL_HANDLE;
            return false;
        }

        WAVE_LOG_INFO("[render] Vulkan surface created.");

        // 3) Physical device

        uint32_t deviceCount = 0;
        result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        if (!check_vk_result(result, "vkEnumeratePhysicalDevices") || deviceCount == 0)
        {
            WAVE_LOG_ERROR("[render] No Vulkan physical devices found.");
            return false;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());
        if (!check_vk_result(result, "vkEnumeratePhysicalDevices(data)"))
        {
            return false;
        }

        auto is_device_suitable = [&](VkPhysicalDevice device) -> bool
        {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

            std::optional<std::uint32_t> graphicsIndex;
            std::optional<std::uint32_t> presentIndex;

            for (uint32_t i = 0; i < queueFamilyCount; ++i)
            {
                const auto& props = families[i];

                if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    graphicsIndex = i;

                VkBool32 presentSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
                if (presentSupport == VK_TRUE)
                    presentIndex = i;

                if (graphicsIndex && presentIndex)
                    break;
            }

            if (!graphicsIndex || !presentIndex)
                return false;

            uint32_t extCount = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
            std::vector<VkExtensionProperties> available(extCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, available.data());

            std::set<std::string> required(kRequiredDeviceExtensions.begin(),
                                           kRequiredDeviceExtensions.end());

            for (const auto& ext : available)
                required.erase(ext.extensionName);

            if (!required.empty())
                return false;

            m_graphicsQueueFamily = *graphicsIndex;
            m_presentQueueFamily  = *presentIndex;
            return true;
        };

        m_physicalDevice = VK_NULL_HANDLE;

        for (auto dev : devices)
        {
            if (is_device_suitable(dev))
            {
                m_physicalDevice = dev;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE)
        {
            WAVE_LOG_ERROR("[render] Failed to find a suitable Vulkan physical device.");
            return false;
        }

        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(m_physicalDevice, &props);
        WAVE_LOG_INFO("[render] Using physical device: ", props.deviceName);

        // 4) Logical device + queues

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<std::uint32_t> uniqueIndices = {
            m_graphicsQueueFamily,
            m_presentQueueFamily
        };

        float queuePriority = 1.0f;
        for (auto index : uniqueIndices)
        {
            VkDeviceQueueCreateInfo qci{};
            qci.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            qci.queueFamilyIndex = index;
            qci.queueCount       = 1;
            qci.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(qci);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        deviceInfo.pQueueCreateInfos       = queueCreateInfos.data();
        deviceInfo.pEnabledFeatures        = &deviceFeatures;
        deviceInfo.enabledExtensionCount   = static_cast<uint32_t>(kRequiredDeviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = kRequiredDeviceExtensions.data();
        deviceInfo.enabledLayerCount       = 0;
        deviceInfo.ppEnabledLayerNames     = nullptr;

        result = vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device);
        if (!check_vk_result(result, "vkCreateDevice"))
        {
            m_device = VK_NULL_HANDLE;
            return false;
        }

        vkGetDeviceQueue(m_device, m_graphicsQueueFamily, 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, m_presentQueueFamily, 0, &m_presentQueue);

        WAVE_LOG_INFO("[render] Vulkan logical device created. "
                      "Graphics queue family = ", m_graphicsQueueFamily,
                      ", Present queue family = ", m_presentQueueFamily);

        // 5) Command pool, swapchain, render pass, framebuffers, command buffers, sync, pipeline

        if (!create_command_pool())
            return false;

        if (!create_swapchain())
        {
            WAVE_LOG_ERROR("[render] Failed to create swapchain.");
            return false;
        }

        if (!create_render_pass())
            return false;

        if (!create_framebuffers())
            return false;

        if (!create_command_buffers())
            return false;

        if (!create_sync_objects())
            return false;

        // Minimal pipeline using simple shaders under resources/shaders/editor
        if (!m_simplePipeline.initialize(
                m_device,
                m_renderPass,
                m_swapchainExtent,
                "shaders/editor/simple.vert.spv",
                "shaders/editor/simple.frag.spv"))
        {
            WAVE_LOG_ERROR("[render] Failed to initialize VkSimplePipeline.");
            return false;
        }

        WAVE_LOG_INFO("[render] Vulkan backend initialized (full frame loop with simple pipeline).");
        return true;
    }

    namespace
    {
        VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats)
        {
            for (const auto& f : formats)
            {
                if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
                    f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return f;
                }
            }

            return formats[0];
        }

        VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& modes)
        {
            for (auto m : modes)
            {
                if (m == VK_PRESENT_MODE_MAILBOX_KHR)
                    return m;
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR& caps,
                                 std::uint32_t width,
                                 std::uint32_t height)
        {
            if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
            {
                return caps.currentExtent;
            }

            VkExtent2D actual{};
            actual.width  = std::max(caps.minImageExtent.width,
                                     std::min(caps.maxImageExtent.width, width));
            actual.height = std::max(caps.minImageExtent.height,
                                     std::min(caps.maxImageExtent.height, height));

            return actual;
        }
    }

    bool VkBackend::create_swapchain()
    {
        VkSurfaceCapabilitiesKHR caps{};
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_physicalDevice, m_surface, &caps);
        if (!check_vk_result(result, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"))
            return false;

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, nullptr);
        if (formatCount == 0)
        {
            WAVE_LOG_ERROR("[render] No surface formats available.");
            return false;
        }
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &formatCount, formats.data());

        uint32_t presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);
        if (presentModeCount == 0)
        {
            WAVE_LOG_ERROR("[render] No present modes available.");
            return false;
        }
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data());

        VkSurfaceFormatKHR surfaceFormat = choose_surface_format(formats);
        VkPresentModeKHR presentMode     = choose_present_mode(presentModes);
        VkExtent2D extent                = choose_extent(caps, m_width, m_height);

        uint32_t imageCount = caps.minImageCount + 1;
        if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
            imageCount = caps.maxImageCount;

        VkSwapchainCreateInfoKHR swapInfo{};
        swapInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapInfo.surface          = m_surface;
        swapInfo.minImageCount    = imageCount;
        swapInfo.imageFormat      = surfaceFormat.format;
        swapInfo.imageColorSpace  = surfaceFormat.colorSpace;
        swapInfo.imageExtent      = extent;
        swapInfo.imageArrayLayers = 1;
        swapInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[2] = { m_graphicsQueueFamily, m_presentQueueFamily };

        if (m_graphicsQueueFamily != m_presentQueueFamily)
        {
            swapInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            swapInfo.queueFamilyIndexCount = 2;
            swapInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            swapInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            swapInfo.queueFamilyIndexCount = 0;
            swapInfo.pQueueFamilyIndices   = nullptr;
        }

        swapInfo.preTransform   = caps.currentTransform;
        swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapInfo.presentMode    = presentMode;
        swapInfo.clipped        = VK_TRUE;
        swapInfo.oldSwapchain   = VK_NULL_HANDLE;

        VkResult resultSwap = vkCreateSwapchainKHR(m_device, &swapInfo, nullptr, &m_swapchain);
        if (!check_vk_result(resultSwap, "vkCreateSwapchainKHR"))
            return false;

        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
        m_swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());

        m_swapchainImageFormat = surfaceFormat.format;
        m_swapchainExtent      = extent;

        m_swapchainImageViews.resize(imageCount);

        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image    = m_swapchainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format   = m_swapchainImageFormat;

            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel   = 0;
            viewInfo.subresourceRange.levelCount     = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount     = 1;

            VkResult viewRes = vkCreateImageView(m_device, &viewInfo, nullptr, &m_swapchainImageViews[i]);
            if (!check_vk_result(viewRes, "vkCreateImageView"))
                return false;
        }

        WAVE_LOG_INFO("[render] Swapchain created with ", imageCount,
                      " images, format ", static_cast<int>(m_swapchainImageFormat),
                      ", extent ", m_swapchainExtent.width, "x", m_swapchainExtent.height);

        return true;
    }

    void VkBackend::destroy_swapchain()
    {
        destroy_framebuffers();

        for (auto view : m_swapchainImageViews)
        {
            if (view != VK_NULL_HANDLE)
                vkDestroyImageView(m_device, view, nullptr);
        }
        m_swapchainImageViews.clear();
        m_swapchainImages.clear();

        if (m_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            m_swapchain = VK_NULL_HANDLE;
        }
    }

    bool VkBackend::create_render_pass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format         = m_swapchainImageFormat;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo rpInfo{};
        rpInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpInfo.attachmentCount = 1;
        rpInfo.pAttachments    = &colorAttachment;
        rpInfo.subpassCount    = 1;
        rpInfo.pSubpasses      = &subpass;
        rpInfo.dependencyCount = 1;
        rpInfo.pDependencies   = &dependency;

        VkResult res = vkCreateRenderPass(m_device, &rpInfo, nullptr, &m_renderPass);
        if (!check_vk_result(res, "vkCreateRenderPass"))
            return false;

        return true;
    }

    void VkBackend::destroy_render_pass()
    {
        if (m_renderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            m_renderPass = VK_NULL_HANDLE;
        }
    }

    bool VkBackend::create_framebuffers()
    {
        m_framebuffers.resize(m_swapchainImageViews.size());

        for (size_t i = 0; i < m_swapchainImageViews.size(); ++i)
        {
            VkImageView attachments[] = { m_swapchainImageViews[i] };

            VkFramebufferCreateInfo fbInfo{};
            fbInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbInfo.renderPass      = m_renderPass;
            fbInfo.attachmentCount = 1;
            fbInfo.pAttachments    = attachments;
            fbInfo.width           = m_swapchainExtent.width;
            fbInfo.height          = m_swapchainExtent.height;
            fbInfo.layers          = 1;

            VkResult res = vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_framebuffers[i]);
            if (!check_vk_result(res, "vkCreateFramebuffer"))
                return false;
        }

        return true;
    }

    void VkBackend::destroy_framebuffers()
    {
        for (auto fb : m_framebuffers)
        {
            if (fb != VK_NULL_HANDLE)
                vkDestroyFramebuffer(m_device, fb, nullptr);
        }
        m_framebuffers.clear();
    }

    bool VkBackend::create_command_pool()
    {
        VkCommandPoolCreateInfo info{};
        info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        info.queueFamilyIndex = m_graphicsQueueFamily;

        VkResult res = vkCreateCommandPool(m_device, &info, nullptr, &m_commandPool);
        if (!check_vk_result(res, "vkCreateCommandPool"))
            return false;

        return true;
    }

    void VkBackend::destroy_command_pool()
    {
        if (m_commandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            m_commandPool = VK_NULL_HANDLE;
        }
    }

    bool VkBackend::create_command_buffers()
    {
        m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo alloc{};
        alloc.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc.commandPool        = m_commandPool;
        alloc.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        VkResult res = vkAllocateCommandBuffers(m_device, &alloc, m_commandBuffers.data());
        if (!check_vk_result(res, "vkAllocateCommandBuffers"))
            return false;

        return true;
    }

    void VkBackend::destroy_command_buffers()
    {
        if (!m_commandBuffers.empty() && m_commandPool != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(m_device,
                                 m_commandPool,
                                 static_cast<uint32_t>(m_commandBuffers.size()),
                                 m_commandBuffers.data());
        }
        m_commandBuffers.clear();
    }

    bool VkBackend::create_sync_objects()
    {
        m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semInfo{};
        semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            if (!check_vk_result(vkCreateSemaphore(m_device, &semInfo, nullptr, &m_imageAvailableSemaphores[i]),
                                 "vkCreateSemaphore(imageAvailable)"))
                return false;

            if (!check_vk_result(vkCreateSemaphore(m_device, &semInfo, nullptr, &m_renderFinishedSemaphores[i]),
                                 "vkCreateSemaphore(renderFinished)"))
                return false;

            if (!check_vk_result(vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]),
                                 "vkCreateFence(inFlight)"))
                return false;
        }

        m_currentFrame = 0;
        return true;
    }

    void VkBackend::destroy_sync_objects()
    {
        for (auto f : m_inFlightFences)
        {
            if (f != VK_NULL_HANDLE)
                vkDestroyFence(m_device, f, nullptr);
        }

        for (auto s : m_imageAvailableSemaphores)
        {
            if (s != VK_NULL_HANDLE)
                vkDestroySemaphore(m_device, s, nullptr);
        }

        for (auto s : m_renderFinishedSemaphores)
        {
            if (s != VK_NULL_HANDLE)
                vkDestroySemaphore(m_device, s, nullptr);
        }

        m_inFlightFences.clear();
        m_imageAvailableSemaphores.clear();
        m_renderFinishedSemaphores.clear();
    }

    bool VkBackend::acquire_next_image()
    {
        VkResult res = vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame],
                                       VK_TRUE, UINT64_MAX);
        if (!check_vk_result(res, "vkWaitForFences"))
            return false;

        res = vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            UINT64_MAX,
            m_imageAvailableSemaphores[m_currentFrame],
            VK_NULL_HANDLE,
            &m_currentImageIndex);

        if (res == VK_ERROR_OUT_OF_DATE_KHR)
        {
            resize(m_width, m_height);
            return false;
        }

        if (!check_vk_result(res, "vkAcquireNextImageKHR"))
            return false;

        res = vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
        if (!check_vk_result(res, "vkResetFences"))
            return false;

        return true;
    }

    bool VkBackend::record_command_buffer(VkCommandBuffer cmd, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkResult res = vkBeginCommandBuffer(cmd, &beginInfo);
        if (!check_vk_result(res, "vkBeginCommandBuffer"))
            return false;

        VkClearValue clearColor{};
        clearColor.color = { { 0.05f, 0.05f, 0.08f, 1.0f } };

        VkRenderPassBeginInfo rpBegin{};
        rpBegin.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpBegin.renderPass        = m_renderPass;
        rpBegin.framebuffer       = m_framebuffers[imageIndex];
        rpBegin.renderArea.offset = {0, 0};
        rpBegin.renderArea.extent = m_swapchainExtent;
        rpBegin.clearValueCount   = 1;
        rpBegin.pClearValues      = &clearColor;

        vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the simple pipeline and draw a triangle.
        if (m_simplePipeline.is_valid())
        {
            vkCmdBindPipeline(cmd,
                              VK_PIPELINE_BIND_POINT_GRAPHICS,
                              m_simplePipeline.handle());

            // No vertex buffer bound: vertex shader uses gl_VertexIndex to generate a fullscreen triangle.
            vkCmdDraw(cmd, 3, 1, 0, 0);
        }

        vkCmdEndRenderPass(cmd);

        res = vkEndCommandBuffer(cmd);
        if (!check_vk_result(res, "vkEndCommandBuffer"))
            return false;

        return true;
    }

    bool VkBackend::present_image()
    {
        VkSemaphore waitSemaphores[]   = { m_renderFinishedSemaphores[m_currentFrame] };
        VkSwapchainKHR swapchains[]    = { m_swapchain };
        uint32_t       imageIndices[]  = { m_currentImageIndex };

        VkPresentInfoKHR present{};
        present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores    = waitSemaphores;
        present.swapchainCount     = 1;
        present.pSwapchains        = swapchains;
        present.pImageIndices      = imageIndices;
        present.pResults           = nullptr;

        VkResult res = vkQueuePresentKHR(m_presentQueue, &present);

        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
        {
            resize(m_width, m_height);
            return false;
        }

        if (!check_vk_result(res, "vkQueuePresentKHR"))
            return false;

        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        return true;
    }

    void VkBackend::begin_frame()
    {
        if (!m_device || !m_swapchain)
            return;

        if (!acquire_next_image())
            return;

        VkCommandBuffer cmd = m_commandBuffers[m_currentFrame];

        vkResetCommandBuffer(cmd, 0);
        record_command_buffer(cmd, m_currentImageIndex);
    }

    void VkBackend::end_frame()
    {
        if (!m_device || !m_swapchain)
            return;

        VkCommandBuffer cmd = m_commandBuffers[m_currentFrame];

        VkSemaphore waitSemaphores[]      = { m_imageAvailableSemaphores[m_currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[]    = { m_renderFinishedSemaphores[m_currentFrame] };

        VkSubmitInfo submit{};
        submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount   = 1;
        submit.pWaitSemaphores      = waitSemaphores;
        submit.pWaitDstStageMask    = waitStages;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &cmd;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores    = signalSemaphores;

        VkResult res = vkQueueSubmit(m_graphicsQueue, 1, &submit, m_inFlightFences[m_currentFrame]);
        if (!check_vk_result(res, "vkQueueSubmit"))
            return;

        present_image();
    }

    void VkBackend::resize(std::uint32_t width, std::uint32_t height)
    {
        m_width  = width;
        m_height = height;

        if (!m_device)
            return;

        WAVE_LOG_INFO("[render] Resizing Vulkan backend to ",
                      width, "x", height);

        vkDeviceWaitIdle(m_device);

        destroy_swapchain();
        destroy_render_pass();
        destroy_framebuffers();

        if (!create_swapchain())
            return;
        if (!create_render_pass())
            return;
        if (!create_framebuffers())
            return;

        // Rebuild the simple pipeline using the new extent.
        m_simplePipeline.shutdown();
        m_simplePipeline.initialize(
            m_device,
            m_renderPass,
            m_swapchainExtent,
            "shaders/editor/simple.vert.spv",
            "shaders/editor/simple.frag.spv");
    }

    void VkBackend::shutdown()
    {
        if (!m_instance && !m_surface && !m_physicalDevice && !m_device && !m_swapchain)
            return;

        WAVE_LOG_INFO("[render] Shutting down Vulkan backend...");

        if (m_device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_device);

            m_simplePipeline.shutdown();

            destroy_sync_objects();
            destroy_command_buffers();
            destroy_command_pool();

            destroy_swapchain();
            destroy_render_pass();

            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }

        if (m_surface != VK_NULL_HANDLE && m_instance != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

        if (m_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }

        m_physicalDevice        = VK_NULL_HANDLE;
        m_graphicsQueue         = VK_NULL_HANDLE;
        m_presentQueue          = VK_NULL_HANDLE;
        m_graphicsQueueFamily   = UINT32_MAX;
        m_presentQueueFamily    = UINT32_MAX;
        m_window_handle         = nullptr;

        WAVE_LOG_INFO("[render] Vulkan backend shutdown complete.");
    }

} // namespace wave::engine::render::vulkan
