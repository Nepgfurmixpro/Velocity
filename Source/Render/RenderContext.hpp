#pragma once

#include <Core/Base.hpp>
#include <Render/RenderAPI.hpp>

#include <spdlog/spdlog.h>

namespace Velocity {
    class RenderContext {
    public:
        virtual ~RenderContext() = default;
        virtual void SwapBuffers() = 0;

        static Ref<RenderContext> Create();
    };
}