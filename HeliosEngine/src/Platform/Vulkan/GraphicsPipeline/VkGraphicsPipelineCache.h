#pragma once


#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/GraphicsPipeline.h"
#include "Helios/Graphics/NativeCommandList.h"
#include "Helios/Resources/Shader.h"
#include "Helios/Resources/ShaderModule.h"
#include "Helios/Utils/DecayingMapCache.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkRenderPassCache.h"

namespace Helios::Internal
{
    struct VkGraphicsPipelineKey
    {
        GraphicsPipelineKey gpKey;
        VkRenderPassCacheKey renderPassKey;
        std::uint32_t subpass = 0u;

        bool operator==(const VkGraphicsPipelineKey& other) const {
            return subpass == other.subpass 
                && gpKey == other.gpKey
                && renderPassKey == other.renderPassKey;
        }
    };

    struct VkGraphicsPipelineKeyHasher
    {
        size_t operator()(const VkGraphicsPipelineKey& key) const noexcept
        {

            XXH64_hash_t hash;
            XXH3_state_t* const state = XXH3_createState();
            XXH3_64bits_reset(state);

            XXH3_64bits_update(state, &key.subpass, sizeof(key.subpass));
            XXH3_64bits_update(state, &key.renderPassKey, sizeof(key.renderPassKey));

            size_t gpHash = std::hash<GraphicsPipelineKey>{}(key.gpKey);

            XXH3_64bits_update(state, &gpHash, sizeof(gpHash));

            hash = XXH3_64bits_digest(state);
            XXH3_freeState(state);

            return static_cast<size_t>(hash);
        }
    };
    
    struct VkGraphicsPipelineSpecification;
    class VkGraphicsPipeline;
    class VkGraphicsPipelineCache
    {
    public:
        
        VkGraphicsPipelineCache(const Ref<VkGraphicsDevice>& device) : m_device(device)  {}

        Ref<VkGraphicsPipeline> Acquire(VkRenderPassCacheKey passKey, std::uint32_t subpass, const BindPipelineInfo& spec);

        void Step() { m_pipelineCache.Step(); }
        std::size_t GetPipelineCacheSize() const { return m_pipelineCache.Size(); }

    private:
        Ref<VkGraphicsDevice> m_device;

        

        static constexpr bool TICK_BASED = true;
        static constexpr std::uint32_t PIPELINE_DECAY_THRESHOLD = 60 * 2;

        DecayingMapCache<VkGraphicsPipelineKey, Ref<VkGraphicsPipeline>, PIPELINE_DECAY_THRESHOLD, TICK_BASED, VkGraphicsPipelineKeyHasher> m_pipelineCache;

        // Per cache instance
        std::mutex m_mutex;
    };
}

#include "VkGraphicsPipelineCache.inl"