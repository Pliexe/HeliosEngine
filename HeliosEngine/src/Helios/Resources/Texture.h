#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	class HELIOS_API Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;
        
        virtual void SetData(void* data, uint32_t size) = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual void* GetTextureID() const = 0;
        
        virtual bool operator==(const Texture& other) const = 0;
        virtual bool operator!=(const Texture& other) const = 0;
    };

    class HELIOS_API Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(const std::string& path);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
    };
}