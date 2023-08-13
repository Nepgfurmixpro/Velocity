#pragma once

#include <Render/Window.hpp>
#include <Core/Assert.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Render/RenderContext.hpp>
#include <Render/Renderer.hpp>

namespace Velocity {
    class VLKWindow : public Window {
    public:
        explicit VLKWindow(const WindowProps& props);
        ~VLKWindow() override;

        void SetName(const std::string& name) override;
        void SetEventCallback(const WindowEventCallback& callback) override;

        uint32_t GetWidth() override;
        uint32_t GetHeight() override;

        void PollEvents() override;
        void OnUpdate() override;

        void* GetNative() override;
    private:
        GLFWwindow* m_Window;

        uint32_t m_Width, m_Height;
        WindowEventCallback m_EventCallback;

        Ref<RenderContext> m_Context;
    };
}