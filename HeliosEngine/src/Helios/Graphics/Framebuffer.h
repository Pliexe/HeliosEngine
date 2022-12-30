#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Translation/Vector.h"
#include "Helios/Resources/Color.h"

namespace Helios
{
    class HELIOS_API Framebuffer
    {
    public:
        enum class Format
        {
            Float4,
            Uint2
        };
    protected:
		Format m_Format;
    public:
		
        Framebuffer() = default;
        //virtual ~Framebuffer() = 0;
		
        virtual void ClearColor(Color color) = 0;

        virtual void Invalidate() = 0;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void AddDepthStencil() = 0;
        virtual void RemoveDepthStencil() = 0;

        virtual bool IsSet() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual Size GetSize() const = 0;

        virtual void* GetTextureID() = 0;

        static Ref<Framebuffer> Create(uint32_t width, uint32_t height);
		static Ref<Framebuffer> Create(uint32_t width, uint32_t height, Format format);
    };
}