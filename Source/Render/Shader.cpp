#include <Render/Shader.hpp>

#include <Render/RenderAPI.hpp>

namespace Velocity {
    Ref<Shader> Shader::Create(const std::string &path, Type type) {
        switch (RenderAPI::GetAPI()) {
            case GraphicsAPI::Vulkan:
                return nullptr;
            default:
                spdlog::warn("The Render API was not chosen. Please choose one before creating any platform specific objects.");
                throw std::runtime_error("Exception while creating Shader");
        }
    }
}