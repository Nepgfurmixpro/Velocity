#include <Core/File.hpp>

namespace Velocity {
    std::vector<char> File::Read(const std::string& filepath) {
        std::ifstream input(filepath, std::ios::ate | std::ios::binary);

        VLY_ASSERT_CRITICAL_LOG(input.is_open(), "Failed top open file.");

        size_t fileSize = static_cast<size_t>(input.tellg());
        std::vector<char> buffer(fileSize);

        input.seekg(0);
        input.read(buffer.data(), fileSize);

        input.close();

        return buffer;
    }
}