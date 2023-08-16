#pragma once

#include <Core/WindowEvent.hpp>
#include <Core/Base.hpp>
#include <Render/RenderAPI.hpp>
#include <Render/Window.hpp>
#include <Render/Renderer.hpp>

namespace Velocity {
    struct ApplicationSettings {
        uint32_t Width;
        uint32_t Height;
        const char* Name;
        GraphicsAPI API;
    };

    class Application {
    public:
        explicit Application(const ApplicationSettings& settings);

        void Run();
        void Close();

        static Application* Get();
        Ref<Window> GetWindow();
        ApplicationSettings& GetSettings();

    private:
        void OnEvent(WindowEvent& event);

        void OnUpdate();

        Ref<Window> m_Window;
        bool m_Running = true;
        ApplicationSettings m_Settings;

        static Application* s_Instance;
    };

    Application* CreateApp();
}