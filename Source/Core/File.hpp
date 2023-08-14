#pragma once

#include <Core/Assert.hpp>
#include <string>
#include <vector>
#include <fstream>

namespace Velocity {
    namespace File {
        std::vector<char> Read(const std::string& filepath);
    }
}