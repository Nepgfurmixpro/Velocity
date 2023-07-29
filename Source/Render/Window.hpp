#pragma once

#include <Core/Base.hpp>
#include <Render/RenderAPI.hpp>
#include <Core/WindowEvent.hpp>

#include <string>
#include <spdlog/spdlog.h>

namespace Velocity {
    struct WindowProps {
        uint32_t Width;
        uint32_t Height;
        const char* Name;
    };

    class Window {
    public:
        virtual ~Window() = default;

        virtual void SetName(const std::string& name) = 0;
        virtual void SetEventCallback(const WindowEventCallback& callback) = 0;

        virtual uint32_t GetWidth() = 0;
        virtual uint32_t GetHeight() = 0;

        virtual void PollEvents() = 0;
        virtual void OnUpdate() = 0;

        static Ref<Window> Create(const WindowProps& props);
    };
}