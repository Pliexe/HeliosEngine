#pragma once

#include "VkSampler.h"

#include "VkGraphicsDevice.h"
#include "VkRenderPassCache.h"
#include "VkFramebufferCache.h"
#include "GraphicsPipeline/VkGraphicsPipelineCache.h"
#include "Sync/VkSyncTracker.h"
#include "Platform/Vulkan/GraphicsPipeline/VkDescriptorSetLayoutCache.h"
#include "Sync/VkSyncPool.h"
#include "Platform/Vulkan/GraphicsPipeline/VkShaderCache.h"
// #include "Platform/Vulkan/GraphicsPipeline/VkDescriptorCache.h"

namespace Helios::Internal
{
    inline VkDescriptorSetCache& VkGraphicsDevice::GetDescriptorSetCache()
    {
        assert(m_descriptorSetCache != nullptr);
        return *m_descriptorSetCache;
    }
    inline VkRenderPassCache& VkGraphicsDevice::GetRenderPassCache()
    {
        assert(m_renderPassCache != nullptr);
        return *m_renderPassCache;
    }
    inline VkFramebufferCache& VkGraphicsDevice::GetFramebufferCache()
    {
        assert(m_framebufferCache != nullptr);
        return *m_framebufferCache;
    }
    inline VkSyncTracker& VkGraphicsDevice::GetSyncTracker()
    {
        assert(m_syncTracker != nullptr);
        return *m_syncTracker;
    }
    inline VkDescriptorSetLayoutCache& VkGraphicsDevice::GetDescriptorSetLayoutCache()
    {
        assert(m_descriptorSetLayoutCache != nullptr);
        return *m_descriptorSetLayoutCache;
    }
    inline VkGraphicsPipelineCache& VkGraphicsDevice::GetGraphicsPipelineCache()
    {
        assert(m_graphicsPipelineCache != nullptr);
        return *m_graphicsPipelineCache;
    }
    
    inline VkSyncPool& VkGraphicsDevice::GetSyncPool()
    {
        assert(m_syncPool != nullptr);
        return *m_syncPool;
    }
    inline VkShaderCache& VkGraphicsDevice::GetShaderCache()
    {
        assert(m_shaderCache != nullptr);
        return *m_shaderCache;
    }
}