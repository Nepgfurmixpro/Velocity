#include <Render/Window.hpp>

// Platform Specific
#include <Platform/Vulkan/VLKWindow.hpp>

namespace Velocity {
    Ref<Window> Window::Create(const WindowProps &props) {
        switch (RenderAPI::GetAPI()) {
            case GraphicsAPI::Vulkan:
                return CreateRef<VLKWindow>(props);
            default:
                spdlog::warn("The Render API was not chosen. Please choose one before creating any platform specific objects");
                throw std::runtime_error("Exception while creating Window");
        }

        return nullptr;
    }
}