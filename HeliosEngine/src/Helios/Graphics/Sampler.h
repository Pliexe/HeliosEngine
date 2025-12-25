#pragma once

#include "pch.h"

namespace Helios
{
    enum class TextureWrap
    {
        Repeat,
        Clamp,
        Mirror,
        Border
    };
    
    enum class TextureFilter
    {
        Nearest,
        Linear,
    };

    enum class MipFilter
    {
        None,
        Nearest,
        Linear,
    };

    struct SamplerConfiguration
    {
        TextureFilter minFilter = TextureFilter::Linear;
        TextureFilter magFilter = TextureFilter::Linear;
        MipFilter mipFilter = MipFilter::Linear;

        TextureWrap wrapU = TextureWrap::Repeat;
        TextureWrap wrapV = TextureWrap::Repeat;
        TextureWrap wrapW = TextureWrap::Repeat;
    };
    
    class Sampler
    {
    public:
        // virtual ~Sampler() = default;
    
        static Ref<Sampler> GetOrCreate(SamplerConfiguration config);
        static void GarbageCollect();
        static void Destroy();
    };
}