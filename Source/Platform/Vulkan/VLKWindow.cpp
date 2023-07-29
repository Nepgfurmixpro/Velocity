#include <Platform/Vulkan/VLKWindow.hpp>

namespace Velocity {
    VLKWindow::VLKWindow(const WindowProps &props) {
        VLY_ASSERT_CRITICAL_LOG(glfwInit(), "Window failed to create. GLFW was unable to initialize.");
        spdlog::debug("GLFW initialized correctly. Continuing with window initialization.");

        m_EventCallback = [](const WindowEvent& ev) {
            spdlog::warn("Window event callback not set. Window events gone without capture.");
        };

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(
                static_cast<int>(props.Width),
                static_cast<int>(props.Height),
                props.Name,
                nullptr,
                nullptr);

        VLY_ASSERT_CRITICAL_LOG(m_Window != nullptr, "Window creation was unsuccessful.");
        spdlog::debug("Created window successfully. Setting up event callbacks.");

        m_Width = props.Width;
        m_Height = props.Height;

        glfwSetWindowUserPointer(m_Window, this);

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            auto self = (VLKWindow*)glfwGetWindowUserPointer(window);
            self->m_EventCallback(WindowEvent{
                EventType::Quit,
                nullptr
            });
        });
    }

    void VLKWindow::PollEvents() {
        glfwPollEvents();
    }

    void VLKWindow::OnUpdate() {

    }

    VLKWindow::~VLKWindow() noexcept {
        spdlog::debug("Destroying window and terminating GLFW.");
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void VLKWindow::SetEventCallback(const WindowEventCallback &callback) {
        m_EventCallback = callback;
    }

    void VLKWindow::SetName(const std::string &name) {
        glfwSetWindowTitle(m_Window, name.data());
    }

    uint32_t VLKWindow::GetWidth() {
        return m_Width;
    }

    uint32_t VLKWindow::GetHeight() {
        return m_Height;
    }
}