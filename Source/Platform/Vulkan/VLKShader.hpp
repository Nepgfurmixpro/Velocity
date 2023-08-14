#pragma once

#include <Render/Shader.hpp>
#include <Core/File.hpp>
#include <vulkan/vulkan.h>

namespace Velocity {
    class VLKShader : public Shader {
    public:
        VLKShader(const std::string& path, Type type);

        VkShaderModule& GetShaderModule();
        VkPipelineShaderStageCreateInfo& GetStageCreateInfo();
    private:
        Type m_Type;
        VkShaderModule m_ShaderModule;
        VkPipelineShaderStageCreateInfo m_StageCreateInfo;
    };
}