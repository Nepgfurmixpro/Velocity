#pragma once

#include <Render/GraphicsPipeline.hpp>
#include <Platform/Vulkan/VLKShader.hpp>

#include <vulkan/vulkan.h>

namespace Velocity {
    class VLKGraphicsPipeline : public GraphicsPipeline {
    public:
        VLKGraphicsPipeline(const std::vector<Ref<Shader>>& shaders);
        ~VLKGraphicsPipeline();

        VkPipeline& GetNativePipeline();
        VkRenderPass& GetRenderPass();
        std::vector<Ref<VLKShader>> GetShaders();
    private:
        void CreateRenderPass();
        void CreateNativePipeline();

        VkPipelineLayout m_PipelineLayout;
        VkRenderPass m_RenderPass;
        VkPipeline m_GraphicsPipeline;

        std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;
                std::vector<Ref<VLKShader>> m_Shaders;
    };
}