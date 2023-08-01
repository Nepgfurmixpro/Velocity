#include <Core/Application.hpp>

namespace Velocity {
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSettings& settings) : m_Settings(settings){
        spdlog::set_pattern("[%T %D] [%n] [%^%l%$] [thread %t] %v");
        spdlog::set_level(spdlog::level::debug);

        RenderAPI::SetAPI(settings.API);

        WindowProps props{
            settings.Width,
            settings.Height,
            settings.Name
        };

        m_Window = Window::Create(props);

        s_Instance = this;
        m_Window->SetEventCallback([this](WindowEvent& event) {
            this->OnEvent(event);
        });

        Renderer::Init();
    }

    void Application::Run() {
        while (m_Running) {
            m_Window->PollEvents();

            this->OnUpdate();
        }
    }

    void Application::OnEvent(WindowEvent &event) {
        switch (event.Type) {
            case EventType::Quit:
                this->Close();
                break;
        }
    }

    void Application::Close() {
        m_Running = false;
    }

    Application* Application::Get() {
        return s_Instance;
    }

    Ref<Window> Application::GetWindow() {
        return m_Window;
    }

    void Application::OnUpdate() {
        m_Window->OnUpdate();
    }

    ApplicationSettings& Application::GetSettings() {
        return m_Settings;
    }
}