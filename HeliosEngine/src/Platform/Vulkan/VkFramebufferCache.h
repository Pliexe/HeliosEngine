#pragma once

#include "Platform/Vulkan/VkRenderPassCache.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "xxh3.h"
#include "Helios/Utils/DecayingMapCache.h"
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{

    struct VkFramebufferCacheKey
    {
        VkFramebufferCacheKey()
        {
            std::memset(this, 0, sizeof(*this));
        }

        ::VkImageView attachments[MAX_ATTACHMENTS];
        std::size_t attachmentCount = 0;
        
        std::uint32_t width;
        std::uint32_t height;
        VkRenderPassCacheKey renderPassCacheKey;

        bool operator==(const VkFramebufferCacheKey& other) const
        {
            return std::memcmp(this, &other, sizeof(VkFramebufferCacheKey)) == 0;
        }
    };

    struct VkFramebufferCacheKeyHasher
    {
        size_t operator()(const VkFramebufferCacheKey& key) const noexcept
        {
            return static_cast<size_t>(XXH3_64bits(&key, sizeof(VkFramebufferCacheKey)));
        }
    };
    
    class VkGraphicsDevice;
    class VkFramebufferCache
    {
    public:
        // VkFramebufferCache() = delete;
        VkFramebufferCache(const Ref<VkGraphicsDevice>& device);
    
        Vulkan::VkFramebufferWrapper& Acquire(const VkFramebufferCacheKey& key);
        std::size_t GetSize() const { return m_cache.Size(); }
        void Clear() { m_cache.Clear(); }
    
    private:
        friend class VkGraphicsDevice;
        void Step() { m_cache.Step(); }

        static constexpr std::uint32_t FRAME_DECAY_THRESHOLD = 30;
        DecayingMapCache<VkFramebufferCacheKey, Vulkan::VkFramebufferWrapper, FRAME_DECAY_THRESHOLD, TICK_BASED, VkFramebufferCacheKeyHasher> m_cache;

        Ref<VkGraphicsDevice> m_device;
    };
}