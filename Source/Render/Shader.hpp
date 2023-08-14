#pragma once

#include <Core/Base.hpp>

#include <string>
#include <spdlog/spdlog.h>

namespace Velocity {
    class Shader {
    public:
        enum Type {
            Vertex,
            Fragment
        };

        static Ref<Shader> Create(const std::string& path, Type type);
    };
}