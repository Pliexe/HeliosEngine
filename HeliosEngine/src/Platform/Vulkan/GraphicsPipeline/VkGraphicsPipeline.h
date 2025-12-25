#pragma once

#include "Helios/Core/Hash.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/GraphicsPipeline.h"
#include "Platform/Vulkan/VkFramebufferCache.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "VKDescriptorCache.h"
#include <unordered_map>

namespace Helios::Internal
{
    struct VkGraphicsPipelineSpecification
    {
        GraphicsPipelineSpecification base;

        VkRenderPassCacheKey renderPassKey;
        std::uint32_t subpass = 0u;
    };
    
    class VkGraphicsPipeline final : public GraphicsPipeline
    {
    public:
        VkGraphicsPipeline(const Ref<VkGraphicsDevice>& device, const VkGraphicsPipelineSpecification& specification);
        ~VkGraphicsPipeline();

        // bool SetTexture2D(LayoutSet set, uint32_t slot, Ref<Texture2D> texture2d) override;
        // bool SetSampler(LayoutSet set, uint32_t slot, Ref<Sampler> sampler) override;
        // bool SetUniformBuffer(LayoutSet set, uint32_t slot, Ref<UnsafeUniformBuffer> uniformBuffer) override;
        // bool SetMaterial(Ref<MaterialInstance> materialInstance) override;

        bool HasBinding(const Ref<ShaderModule>& shaderModule, std::uint32_t slot, BindingType type)
        {
            Hash128Key key(shaderModule->GetHash().low64, shaderModule->GetHash().high64);
            auto it = m_moduleToSetSlot.find(key);
            if (it == m_moduleToSetSlot.end()) return false;
            return HasBinding(it->second[0], slot, type);
        }

        bool HasBinding(std::uint32_t set, std::uint32_t slot, BindingType type)
        {
            if (!m_pipelineBindingLayout.contains(set)) return false;
            auto& layout = m_pipelineBindingLayout[set];
            if (!layout.contains(type)) return false;
            return layout[type].contains(slot);
        }

        bool IsValid(std::uint32_t set, std::uint32_t slot, BindingType type, std::size_t count = 1)
        {
            // if (m_pipelineBindingLayout.size() <= set) return false;
            if (!m_pipelineBindingLayout.contains(set)) return false;
            auto& layout = m_pipelineBindingLayout[set];
            if (!layout.contains(type)) return false;
            auto it = layout[type].find(slot);
            if (it == layout[type].end()) return false;

            return it->second.count == count;
        }

        bool IsValid(const Ref<ShaderModule>& shaderModule, std::uint32_t slot, BindingType type, std::size_t count = 1)
        {
            // TODO: This is bad, make a constructor for taking it in, it is inconsistent and could be mixed by mistake!
            Hash128Key key(shaderModule->GetHash().low64, shaderModule->GetHash().high64);
            auto it = m_moduleToSetSlot.find(key);
            if (it == m_moduleToSetSlot.end()) return false;
            // all indecies will use same layout anyways so no need to check all
            return IsValid(it->second[0], slot, type, count);
        }

        const std::vector<std::uint32_t>& GetSetsForModule(const Ref<ShaderModule>& shaderModule) 
        {
            // TODO: This is bad, make a constructor for taking it in, it is inconsistent and could be mixed by mistake!
            Hash128Key key(shaderModule->GetHash().low64, shaderModule->GetHash().high64);
            static const std::vector<std::uint32_t> empty{};
            auto it = m_moduleToSetSlot.find(key);
            if (it == m_moduleToSetSlot.end()) return empty;
            return it->second;
        }

    private:
        friend class VkBindingSet;
        friend class VkCommandList;    
        friend class VkCommandListImmediate;

        std::vector<Ref<Vulkan::VkDescriptorSetLayoutWrapper>> m_layouts;

        using SetSlot = std::uint32_t;
        using BindingSlotToInfo = std::unordered_map<std::uint32_t, BindingInfo>;
        using BindingTypeToSlot = std::unordered_map<BindingType, BindingSlotToInfo>;
        std::unordered_map<SetSlot, BindingTypeToSlot> m_pipelineBindingLayout;
        std::unordered_map<Hash128Key, std::vector<SetSlot>, Hash128KeyHasher, Hash128KeyEqual> m_moduleToSetSlot;

        std::unordered_map<SetSlot, BindingLayout> m_mergedBindingLayouts;
        
        VkPipelineLayout m_pipelineLayout;
        VkPipeline m_pipeline;

        Ref<VertexShader> m_vertexShader;
        Ref<PixelShader> m_pixelShader;
        Ref<GeometryShader> m_geometryShader;

        Ref<VkGraphicsDevice> m_device;
    };
}