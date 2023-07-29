#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <spdlog/spdlog.h>
#include <optional>

namespace Velocity::VLK {
#ifdef VELOCITY_DEBUG
    const bool EnableValidationLayers = true;
#else
    const bool EnableValidationLayers = false;
#endif

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool IsComplete();
    };

    extern std::vector<const char*> ValidationLayers;

    bool CheckValidationLayerSupport();

    VkResult ValidateResult(VkResult result, bool fatal, const std::string& msg);

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
            VkDebugUtilsMessageTypeFlagsEXT msgType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

    VkResult CreateDebugUtilsMessenger(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator);

    void PopulateMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
    uint32_t ScoreGPU(VkPhysicalDevice device);
    bool IsDeviceSuitable(VkPhysicalDevice device);

    std::string GetDeviceName(VkPhysicalDevice device);

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
}