#pragma once

#include "Helios/Graphics/GPURenderPass.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "xxh3.h"
#include "Helios/Utils/DecayingMapCache.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    constexpr std::size_t MAX_ATTACHMENTS = 8;
    constexpr std::size_t MAX_SUBPASSES = 4;

    struct VkRenderPassCacheKey
    {
        VkRenderPassCacheKey()
        {
            std::memset(this, 0, sizeof(*this));
        }

        VkRenderPassCacheKey(const RenderPassBeginInfo& beginInfo);
        
        struct Attachment
        {
            VkFormat format = VK_FORMAT_UNDEFINED;
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        };

        struct Subpass
        {
            std::uint32_t colorAttachmentCount = 0;
            std::uint32_t colorAttachments[MAX_ATTACHMENTS - 1] = {};
            std::int32_t depthStencilAttachment = -1;
        };

        Attachment attachments[MAX_ATTACHMENTS];
        std::size_t attachmentCount = 0;

        Subpass subpasses[MAX_SUBPASSES];
        std::size_t subpassCount = 0;

        bool operator==(const VkRenderPassCacheKey& other) const
        {
            return std::memcmp(this, &other, sizeof(VkRenderPassCacheKey)) == 0;
        }
    };

    struct VkRenderPassCacheKeyHasher
    {
        size_t operator()(const VkRenderPassCacheKey& key) const noexcept
        {
            return static_cast<size_t>(XXH3_64bits(&key, sizeof(VkRenderPassCacheKey)));
        }
    };

    constexpr bool TICK_BASED = true;
    
    class VkGraphicsDevice;
    class VkRenderPassCache
    {
    public:
        VkRenderPassCache() = delete;
        VkRenderPassCache(const Ref<VkGraphicsDevice>& device) : m_device(device) { }
    
        Vulkan::VkRenderPassWrapper& Acquire(const VkRenderPassCacheKey& key);
        std::size_t GetSize() const { return m_cache.Size(); }

        void Invalidate(const VkRenderPassCacheKey& key) { m_cache.Erase(key); }

        void Clear() { m_cache.Clear(); }
    
    private:
        friend class VkGraphicsDevice;
        void Step() { m_cache.Step(); }

        static constexpr std::uint32_t FRAME_DECAY_THRESHOLD = 30;
        DecayingMapCache<VkRenderPassCacheKey, Vulkan::VkRenderPassWrapper, FRAME_DECAY_THRESHOLD, TICK_BASED, VkRenderPassCacheKeyHasher> m_cache;
        
        Ref<VkGraphicsDevice> m_device;
    };
}