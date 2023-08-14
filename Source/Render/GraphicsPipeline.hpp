#pragma once

#include <Render/Shader.hpp>

namespace Velocity {
    class GraphicsPipeline {
    public:

        static Scope<GraphicsPipeline> Create(const std::vector<Ref<Shader>>& shaders);
    };
}