#include <Platform/Vulkan/VLKShader.hpp>

#include <Render/Renderer.hpp>
#include <Platform/Vulkan/VLKRenderContext.hpp>

namespace Velocity {
    VLKShader::VLKShader(const std::string &path, Type type) : m_Type(type) {
        auto data = File::Read(path);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = data.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        auto renderer = std::reinterpret_pointer_cast<VLKRenderContext>(Renderer::GetInternalContext());

        VLK::ValidateResult(
                vkCreateShaderModule(renderer->GetDevice(), &createInfo, nullptr, &m_ShaderModule),
                true,
                fmt::format("Failed to create shader module for file {0}", path)
                );

        m_StageCreateInfo = {};
        m_StageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        switch (type) {
            case Shader::Vertex: m_StageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            case Shader::Fragment: m_StageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        m_StageCreateInfo.module = m_ShaderModule;
        m_StageCreateInfo.pName = "main";
    }

    VkShaderModule& VLKShader::GetShaderModule() {
        return m_ShaderModule;
    }

    VkPipelineShaderStageCreateInfo& VLKShader::GetStageCreateInfo() {
        return m_StageCreateInfo;
    }
}