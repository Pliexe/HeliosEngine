#pragma once

#include "Color.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Logger.h"
#include "Helios/Graphics/Sampler.h"
#include "Texture.h"


namespace Helios
{
class HELIOS_API MaterialInstance
{
  public:
    Color AlbedoColor;
    Ref<Texture> texture;
    SamplerConfiguration sampler;

  public:
    enum class Type
    {
        Warp,
        Mirror,
        Clamp,
        Border,
        MirrorOnce
    };

    enum class Filter
    {
        MinMagMipPoint,
        ComparisonMinMagMipLinear,
    };

    static Ref<MaterialInstance> Create(Filter filter, Type type);
};
} // namespace Helios