#include <Render/RenderContext.hpp>

// Platform Specific
#include <Platform/Vulkan/VLKRenderContext.hpp>

namespace Velocity {
    Ref<RenderContext> RenderContext::Create() {
        switch (RenderAPI::GetAPI()) {
            case GraphicsAPI::Vulkan:
                return CreateRef<VLKRenderContext>();
            default:
                spdlog::warn("The Render API was not chosen. Please choose one before creating any platform specific objects");
                throw std::runtime_error("Exception while creating RenderContext");
        }

        return nullptr;
    }
}