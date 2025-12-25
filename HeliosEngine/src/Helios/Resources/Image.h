#pragma once

#include "Helios/Math/Vector.h"
#include "pch.h"

#include "Helios/Core/Base.h"

namespace Helios
{
    class HELIOS_API Image
    {
    public:
        enum class Usage : uint8
        {
            None              = 0,
            Sampled           = 1 << 0,
            Storage           = 1 << 1,
            ColorAttachment   = 1 << 2,
            DepthStencil      = 1 << 3,
            TransferSrc       = 1 << 4,
            TransferDst       = 1 << 5,
            Swapchain         = 1 << 6,
        };
    
        enum class Format
        {
            None,

            // 32-bit float formats
            R32G32B32A32_FLOAT,
            R32G32B32_FLOAT,
            R32G32_FLOAT,
            R32_FLOAT,

            // 32-bit signed int formats
            R32G32B32A32_SINT,
            R32G32B32_SINT,
            R32G32_SINT,
            R32_SINT,

            // 32-bit unsigned int formats
            R32G32B32A32_UINT,
            R32G32B32_UINT,
            R32G32_UINT,
            R32_UINT,

            // 16-bit float formats
            R16G16B16A16_FLOAT,
            R16G16_FLOAT,
            R16_FLOAT,

            // 16-bit unsigned normalized formats
            R16G16B16A16_UNORM,
            R16G16_UNORM,
            R16_UNORM,

            // 8-bit unsigned normalized formats
            B8G8R8A8_UNORM,
            R8G8B8A8_UNORM,
            // R8G8B8_UNORM, // Not commonly supported
            R8G8_UNORM,
            R8_UNORM,

            // Depth and stencil
            D24_UNORM_S8_UINT,
            D32_FLOAT,
            
        };

        virtual ~Image() = default;

        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual void SetData(void* data, uint32 size) = 0;

        virtual Size GetSize() const = 0;
        virtual Format GetFormat() const = 0;
        virtual Usage GetUsage() const = 0;

        virtual void* GetID() = 0;

        virtual bool IsDepth() const = 0;
        virtual bool IsSwapchain() const = 0;

        size_t CalculateSize() const;

    protected:
        Format m_format = Format::None;
        Usage m_usage = Usage::None;
		uint32 m_width = 50u;
		uint32 m_height = 50u;
    };

    inline Image::Usage operator|(Image::Usage a, Image::Usage b)
    {
        return static_cast<Image::Usage>(static_cast<uint8>(a) | static_cast<uint8>(b));
    }

    inline Image::Usage operator&(Image::Usage a, Image::Usage b)
    {
        return static_cast<Image::Usage>(static_cast<uint8>(a) & static_cast<uint8>(b));
    }

    inline Image::Usage& operator|=(Image::Usage& a, Image::Usage b)
    {
        a = a | b;
        return a;
    }

    inline Image::Usage& operator&=(Image::Usage& a, Image::Usage b)
    {
        a = a & b;
        return a;
    }

    inline Image::Usage operator~(Image::Usage a)
    {
        return static_cast<Image::Usage>(~static_cast<uint8>(a));
    }
}