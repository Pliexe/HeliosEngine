#include "VkGraphicsPipelineCache.h"
#include "Helios/Core/Exceptions.h"

#include "Helios/Resources/Shader.h"
#include "VkShaderCache.h"

namespace Helios::Internal
{
    Ref<VkGraphicsPipeline> VkGraphicsPipelineCache::Acquire(VkRenderPassCacheKey passKey, std::uint32_t subpass, const BindPipelineInfo& spec)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        VkGraphicsPipelineKey key = 
        {
            .gpKey = {
                .vertexShaderKey = ShaderKey(ShaderModule::Stage::Vertex, spec.vertexShaderModules),
                .pixelShaderKey = ShaderKey(ShaderModule::Stage::Pixel, spec.pixelShaderModules),
                .topology = spec.topology,
                .cullMode = spec.cullMode,
                .fillMode = spec.fillMode,
            }
        };
        key.subpass = subpass;
        key.renderPassKey = passKey;

        if (spec.geometryShaderModules.size() > 0) key.gpKey.geometryShaderKey = ShaderKey(ShaderModule::Stage::Geometry, spec.geometryShaderModules);
        
        auto valPtr = m_pipelineCache.Get(key);
        if (valPtr != nullptr) return *valPtr;

        VkGraphicsPipelineSpecification pipelineInfo;
        pipelineInfo.base.cullMode = key.gpKey.cullMode;
        pipelineInfo.base.fillMode = key.gpKey.fillMode;
        pipelineInfo.base.topology = key.gpKey.topology;

        try {
            pipelineInfo.base.vertexShader = m_device->GetShaderCache().Acquire<Helios::VertexShader>(spec.vertexShaderModules);
            pipelineInfo.base.pixelShader = m_device->GetShaderCache().Acquire<Helios::PixelShader>(spec.pixelShaderModules);
            if (spec.geometryShaderModules.size() > 0)
                pipelineInfo.base.geometryShader = m_device->GetShaderCache().Acquire<Helios::GeometryShader>(spec.geometryShaderModules);
        } catch (const HeliosException& e)
        {
            std::cout << e.GetMessage() << std::endl;
            return nullptr;
        }

        pipelineInfo.subpass = subpass;
        pipelineInfo.renderPassKey = passKey;


        auto pipeline = CreateRef<VkGraphicsPipeline>(m_device, pipelineInfo);
        m_pipelineCache.Insert(key, pipeline);
        return pipeline;
    }
}