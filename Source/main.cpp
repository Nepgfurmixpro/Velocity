#include <Render/Window.hpp>
#include <Render/Renderer.hpp>

using namespace Velocity;

int main() {
    spdlog::set_pattern("[%T %D] [%n] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug);

    RenderAPI::SetAPI(GraphicsAPI::Vulkan);

    try {
        WindowProps props{};
        props.Width = 1280;
        props.Height = 780;
        props.Name = "Velocity";
        Ref<Window> window = Window::Create(props);

        bool running = true;
        window->SetEventCallback([&running](const WindowEvent& event) {
            switch (event.Type) {
                case EventType::Quit:
                    running = false;
                    break;
            }
        });

        Renderer::Init();

        while (running) {
            window->PollEvents();
        }
    } catch (const std::exception& e) {
        spdlog::critical("Exception: {0}", e.what());
    }

    return 0;
}
