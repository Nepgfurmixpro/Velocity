#include <Render/GraphicsPipeline.hpp>

#include <Platform/Vulkan/VLKGraphicsPipeline.hpp>
#include <Render/RenderAPI.hpp>

namespace Velocity {
    Scope<GraphicsPipeline> GraphicsPipeline::Create(const std::vector<Ref<Shader>> &shaders) {
        switch (RenderAPI::GetAPI()) {
            case GraphicsAPI::Vulkan:
                break;
        }
    }
}