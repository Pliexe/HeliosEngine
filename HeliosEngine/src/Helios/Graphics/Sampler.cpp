#include "Sampler.h"

#include "Helios/Graphics/Graphics.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkSampler.h"
#include <memory>

template<>
struct std::hash<Helios::SamplerConfiguration>
{
    std::size_t operator()(const Helios::SamplerConfiguration& config) const
    {
        return (static_cast<std::size_t>(config.minFilter) << 0) ^
                (static_cast<std::size_t>(config.magFilter) << 2) ^
                (static_cast<std::size_t>(config.mipFilter) << 4) ^
                (static_cast<std::size_t>(config.wrapU) << 6) ^
                (static_cast<std::size_t>(config.wrapV) << 8) ^
                (static_cast<std::size_t>(config.wrapW) << 10);
    }
};

namespace Helios
{
    inline bool operator==(const Helios::SamplerConfiguration& lhs, const Helios::SamplerConfiguration& rhs)
    {
        return lhs.minFilter == rhs.minFilter &&
            lhs.magFilter == rhs.magFilter &&
            lhs.mipFilter == rhs.mipFilter &&
            lhs.wrapU == rhs.wrapU &&
            lhs.wrapV == rhs.wrapV &&
            lhs.wrapW == rhs.wrapW;
    }

    static std::unordered_map<SamplerConfiguration, Ref<Sampler>> m_samplerCache;


    
    Ref<Sampler> Sampler::GetOrCreate(SamplerConfiguration config)
    {
        auto it = m_samplerCache.find(config);
        if (it != m_samplerCache.end())
            return it->second;

        Ref<Sampler> sampler = CreateRef<Internal::VkSampler>(std::static_pointer_cast<Internal::VkGraphicsDevice>(Graphics::GetMainDevice()), config);
        m_samplerCache[config] = sampler;
        return sampler;
    }
    
    void Sampler::GarbageCollect()
    {
        for (auto it = m_samplerCache.begin(); it != m_samplerCache.end(); )
    {
        if (it->second.use_count() == 1)
            it = m_samplerCache.erase(it);
        else
            ++it;
    }
    }

    void Sampler::Destroy()
    {
        for (auto& [key, val] : m_samplerCache)
        {
            val.reset();
        }
        m_samplerCache.clear();
    }
}