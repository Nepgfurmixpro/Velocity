#include <Platform/Vulkan/VLKUtils.hpp>

#include <Render/Renderer.hpp>
#include <Core/Application.hpp>
#include <Platform/Vulkan/VLKRenderContext.hpp>

namespace Velocity::VLK {
    std::vector<const char*> ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char*> DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool CheckValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : ValidationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
            VkDebugUtilsMessageTypeFlagsEXT msgType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData) {
        switch (msgSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                spdlog::error("Vulkan Debug: {0}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                spdlog::warn("Vulkan Debug: {0}", pCallbackData->pMessage);
                break;
            default:
                spdlog::debug("Vulkan Debug: {0}", pCallbackData->pMessage);
                break;
        }

        return VK_FALSE;
    }

    VkResult ValidateResult(VkResult result, bool fatal, const std::string& msg) {
        if (result != VK_SUCCESS) {
            if (!fatal) {
                spdlog::warn("VkResult returned not VK_SUCCESS. {0}", msg);
            } else {
                spdlog::critical("VkResult returned not VK_SUCCESS. {0}", msg);
                throw std::runtime_error("VkResult returned not VK_SUCCESS");
            }
        }

        return result;
    }

    VkResult CreateDebugUtilsMessenger(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks *pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void PopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo) {
        if (pCreateInfo != nullptr) {
            pCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            pCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            pCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            pCreateInfo->pfnUserCallback = VLK::DebugCallback;
        }
    }

    bool IsDeviceSuitable(VkPhysicalDevice device) {
        auto indices = FindQueueFamilies(device);

        bool supportsExtensions = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (supportsExtensions) {
            auto swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return indices.IsComplete() && supportsExtensions && swapChainAdequate;
    }

    uint32_t ScoreGPU(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        uint32_t score = 0;

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            score += 50;
        }

        VkPhysicalDeviceMemoryProperties memoryProps{};
        vkGetPhysicalDeviceMemoryProperties(device, &memoryProps);
        std::vector<VkMemoryHeap> heaps(memoryProps.memoryHeaps, memoryProps.memoryHeaps + memoryProps.memoryHeapCount);

        for (const auto& heap : heaps) {
            if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                score += heap.size / 1048576; // heap.size / bytes to mb conversion
                break;
            }
        }

        if (!deviceFeatures.geometryShader) {
            score = 0;
        }

        return score;
    }

    std::string GetDeviceName(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        return {deviceProperties.deviceName};
    }

    bool QueueFamilyIndices::IsComplete() const {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices{};
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;

        auto ctx = (VLKRenderContext*) Renderer::GetInternalContext().get();

        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, ctx->GetSurface(), &presentSupport);

            if (presentSupport) {
                indices.PresentFamily = i;
            }

            if (indices.IsComplete()) break;

            i++;
        }
        return indices;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;
        auto ctx = (VLKRenderContext*) Renderer::GetInternalContext().get();

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, ctx->GetSurface(), &details.Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, ctx->GetSurface(), &formatCount, nullptr);
        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, ctx->GetSurface(), &formatCount, details.Formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, ctx->GetSurface(), &presentModeCount, nullptr);
        if (formatCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, ctx->GetSurface(), &presentModeCount, details.PresentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto& format : availableFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return format;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto& presentMode : availablePresentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) return presentMode;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            auto window = Application::Get()->GetWindow();
            glfwGetFramebufferSize((GLFWwindow *) window->GetNative(), &width, &height);

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}