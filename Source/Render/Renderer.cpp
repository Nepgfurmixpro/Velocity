#include <Render/Renderer.hpp>

namespace Velocity {
    Ref<RenderContext> Renderer::s_Context = nullptr;

    void Renderer::Init() {
        s_Context = RenderContext::Create();
    }

    Ref<RenderContext> Renderer::GetInternalContext() {
        return s_Context;
    }
}