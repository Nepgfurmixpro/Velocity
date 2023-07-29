#pragma once

#include <Render/RenderContext.hpp>
#include <Platform/Vulkan/VLKUtils.hpp>
#include <Core/Assert.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>

namespace Velocity {
    class VLKRenderContext : public RenderContext {
    public:
        VLKRenderContext();
        ~VLKRenderContext();

        void SwapBuffers() override;

        VkInstance& GetInstance();
    private:
        void CreateInstance();
        void CreateDebugMessenger();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSurface();

        std::vector<const char*> GetExtensions();

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device;

        VkQueue m_GraphicsQueue;
    };
}