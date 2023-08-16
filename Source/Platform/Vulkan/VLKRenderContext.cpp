#include <Platform/Vulkan/VLKRenderContext.hpp>

#include <Core/Application.hpp>

namespace Velocity {
    VLKRenderContext::VLKRenderContext() {

    }

    void VLKRenderContext::Init() {
        spdlog::debug("Initializing the Vulkan Render Context");
        this->CreateInstance();
        this->CreateDebugMessenger();
        this->CreateSurface();
        this->PickPhysicalDevice();
        this->CreateLogicalDevice();
        this->CreateSwapchain();
        this->CreateImageViews();
    }

    VLKRenderContext::~VLKRenderContext() noexcept {
        spdlog::debug("Cleaning up VLKRenderContext");
        for (auto imageView : m_SwapchainImageViews) {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        vkDestroyDevice(m_Device, nullptr);

        if (VLK::EnableValidationLayers) {
            VLK::DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void VLKRenderContext::SwapBuffers() {

    }

    void VLKRenderContext::CreateInstance() {
        spdlog::debug("Are validation layers enabled? {0}", VLK::EnableValidationLayers ? "Yes" : "No");
        if (!VLK::CheckValidationLayerSupport() && VLK::EnableValidationLayers) {
            spdlog::error("Validation layers requested but were not found.");
            throw std::runtime_error("Failure while creating Vulkan instance");
        } else if (VLK::EnableValidationLayers) {
            spdlog::debug("Validation layers were found.");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        auto app = Application::Get();
        appInfo.pApplicationName = app->GetSettings().Name;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Velocity";
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = this->GetExtensions();

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        uint32_t layerCount = 0;
        if (VLK::EnableValidationLayers) {
            layerCount = static_cast<uint32_t>(VLK::ValidationLayers.size());
            createInfo.ppEnabledLayerNames = VLK::ValidationLayers.data();
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            VLK::PopulateMessengerCreateInfo(&debugCreateInfo);

            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }
        createInfo.enabledLayerCount = layerCount;

        VLK::ValidateResult(vkCreateInstance(&createInfo, nullptr, &m_Instance),
                            true,
                            "Failed to create Vulkan instance");

        spdlog::debug("Vulkan instance created. Created with {0} extension(s) and {1} layer(s).", extensions.size(), layerCount);

        spdlog::debug("Enabled Extensions:");
        for (size_t i = 0; i < extensions.size(); i++) {
            spdlog::debug("\t#{0}: {1}", i, extensions[i]);
        }

        spdlog::debug("Available Extensions:");
        for (size_t i = 0; i < availableExtensions.size(); i++) {
            spdlog::debug("\t#{0}: {1}", i, availableExtensions[i].extensionName);
        }

        spdlog::debug("Enabled Layers:");
        for (size_t i = 0; i < VLK::ValidationLayers.size(); i++) {
            spdlog::debug("\t#{0}: {1}", i, VLK::ValidationLayers[i]);
        }
    }

    VkInstance& VLKRenderContext::GetInstance() {
        return m_Instance;
    }

    std::vector<const char*> VLKRenderContext::GetExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (VLK::EnableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void VLKRenderContext::CreateDebugMessenger() {
        if (!VLK::EnableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        VLK::PopulateMessengerCreateInfo(&createInfo);

        VLK::ValidateResult(VLK::CreateDebugUtilsMessenger(m_Instance, &createInfo, nullptr, &m_DebugMessenger), false, "Failed to create debug messenger");
    }

    void VLKRenderContext::PickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        VLY_ASSERT_CRITICAL_LOG(deviceCount != 0, "Device count 0 while looking for physical device.");
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        std::multimap<uint32_t, VkPhysicalDevice> candidates;

        for (const auto& device : devices) {
            if (VLK::IsDeviceSuitable(device)) {
                uint32_t score = VLK::ScoreGPU(device);
                candidates.insert(std::make_pair(score, device));

                spdlog::debug("Found GPU {0}. Scored {1}.", VLK::GetDeviceName(device), score);
            } else {
                spdlog::debug("Found GPU {0} but it's not supported.", VLK::GetDeviceName(device));
            }
        }

        if (candidates.rbegin()->first > 0) {
            m_PhysicalDevice = candidates.rbegin()->second;
            spdlog::debug("Picked GPU {0}", VLK::GetDeviceName(m_PhysicalDevice));
        } else {
            spdlog::critical("No GPU found that will work with this engine.");
            throw std::runtime_error("No GPU found");
        }
    }

    void VLKRenderContext::CreateLogicalDevice() {
        VLK::QueueFamilyIndices indices = VLK::FindQueueFamilies(m_PhysicalDevice);

        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueFamilies = {indices.PresentFamily.value(), indices.GraphicsFamily.value()};

        for (auto& queueFamily : uniqueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.geometryShader = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.ppEnabledExtensionNames = VLK::DeviceExtensions.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(VLK::DeviceExtensions.size());

        if (VLK::EnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VLK::ValidationLayers.size());
            createInfo.ppEnabledLayerNames = VLK::ValidationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VLK::ValidateResult(
                vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device),
                true,
                "Failed to create physical device.");

        vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
    }

    void VLKRenderContext::CreateSurface() {
        auto app = Application::Get();
        VLK::ValidateResult(
                glfwCreateWindowSurface(
                        m_Instance,
                        (GLFWwindow*)app->GetWindow()->GetNative(),
                        nullptr, &m_Surface), false, "Failed to create window surface"
                );
    }

    VkSurfaceKHR &VLKRenderContext::GetSurface() {
        return m_Surface;
    }

    void VLKRenderContext::CreateSwapchain() {
        VLK::SwapChainSupportDetails swapchainSupport = VLK::QuerySwapChainSupport(m_PhysicalDevice);

        VkSurfaceFormatKHR  surfaceFormat = VLK::ChooseSwapSurfaceFormat(swapchainSupport.Formats);
        VkPresentModeKHR presentMode = VLK::ChooseSwapPresentMode(swapchainSupport.PresentModes);
        VkExtent2D extent = VLK::ChooseSwapExtent(swapchainSupport.Capabilities);

        uint32_t imageCount = swapchainSupport.Capabilities.minImageCount + 1;

        if (swapchainSupport.Capabilities.maxImageCount > 0 && imageCount > swapchainSupport.Capabilities.maxImageCount) {
            imageCount = swapchainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VLK::QueueFamilyIndices indices = VLK::FindQueueFamilies(m_PhysicalDevice);
        uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

        if (indices.GraphicsFamily != indices.PresentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VLK::ValidateResult(
                vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain),
                true,
                "Failed to create swapchain");

        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr);
        m_SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_SwapchainImages.data());

        m_SwapchainImageFormat = surfaceFormat.format;
        m_SwapchainExtent = extent;
    }

    void VLKRenderContext::CreateImageViews() {
        m_SwapchainImageViews.resize(m_SwapchainImages.size());

        for (size_t i = 0; i < m_SwapchainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapchainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VLK::ValidateResult(
                    vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapchainImageViews[i]),
                    true,
                    "Failed to create image views."
                    );
        }
    }

    void VLKRenderContext::CreateGraphicsPipeline() {

    }

    VkDevice &VLKRenderContext::GetDevice() {
        return m_Device;
    }

    VkExtent2D &VLKRenderContext::GetSwapchainExtent() {
        return m_SwapchainExtent;
    }

    VkFormat &VLKRenderContext::GetSwapchainFormat() {
        return m_SwapchainImageFormat;
    }
}