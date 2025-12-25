#include "VkFramebufferCache.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"

namespace Helios::Internal
{
    VkFramebufferCache::VkFramebufferCache(const Ref<VkGraphicsDevice>& device) : m_device(device) { }
    
    Vulkan::VkFramebufferWrapper& VkFramebufferCache::Acquire(const VkFramebufferCacheKey& key)
    {
        if (m_cache.Contains(key)) return m_cache.GetRef(key);

        m_device->GetRenderPassCache().Invalidate(key.renderPassCacheKey);

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_device->GetRenderPassCache().Acquire(key.renderPassCacheKey).Get();
        framebufferInfo.attachmentCount = key.attachmentCount;
        framebufferInfo.pAttachments = key.attachments;
        framebufferInfo.width = key.width;
        framebufferInfo.height = key.height;
        framebufferInfo.layers = 1;

        auto result = Vulkan::VkFramebufferWrapper::Create(*m_device, &framebufferInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create VkFramebuffer!");

        return m_cache.Insert(key, std::move(result.value()));
    }
}