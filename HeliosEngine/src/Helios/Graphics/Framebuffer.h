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
            NONE,
            R32G32B32A32F,
            R32G32B32F,
            R32G32F,
            R32F,

			R8B8G8A8_UNORM,
			
			D24S8,
            D32F
        };
    protected:
		Format m_Format;
		uint32_t m_Width;
		uint32_t m_Height;		
    public:
		
        Framebuffer() = default;
        //virtual ~Framebuffer() = 0;
		
        virtual void ClearBuffer(unsigned int bufferIndex, Color color) = 0;
        virtual void ClearDepthStencil() = 0;

        virtual void Invalidate() = 0;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual Size GetSize() const = 0;

        virtual void* GetTextureID(unsigned int bufferIndex) = 0;

		virtual Color GetPixel(uint32_t attachment, uint32_t x, uint32_t y) = 0;

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }
		
        static Ref<Framebuffer> Create(uint32_t width, uint32_t hffeight);
		static Ref<Framebuffer> Create(uint32_t width, uint32_t height, std::initializer_list<Format> bufferSpecifications);
    };
}