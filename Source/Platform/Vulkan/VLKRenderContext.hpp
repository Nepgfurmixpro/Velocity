#pragma once

#include <Render/RenderContext.hpp>
#include <Platform/Vulkan/VLKUtils.hpp>
#include <Core/Assert.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
#include <set>

namespace Velocity {
    class VLKRenderContext : public RenderContext {
    public:
        VLKRenderContext();
        ~VLKRenderContext();

        void Init() override;
        void SwapBuffers() override;

        VkInstance& GetInstance();
        VkSurfaceKHR& GetSurface();
        VkDevice& GetDevice();

        VkExtent2D& GetSwapchainExtent();
        VkFormat& GetSwapchainFormat();
    private:
        void CreateInstance();
        void CreateDebugMessenger();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSurface();
        void CreateSwapchain();
        void CreateImageViews();
        void CreateGraphicsPipeline();

        std::vector<const char*> GetExtensions();

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        VkSurfaceKHR m_Surface;

        VkSwapchainKHR m_Swapchain;
        std::vector<VkImage> m_SwapchainImages;
        VkFormat m_SwapchainImageFormat;
        VkExtent2D m_SwapchainExtent;

        std::vector<VkImageView> m_SwapchainImageViews;
    };
}