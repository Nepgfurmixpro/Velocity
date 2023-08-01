#pragma once

#include <Core/Base.hpp>
#include <Render/RenderContext.hpp>
#include <Core/Assert.hpp>

namespace Velocity {
    class Renderer {
    public:
        static void Init();
        static void Cleanup();

        static Ref<RenderContext> GetInternalContext();
    private:
        static Ref<RenderContext> s_Context;
    };
}