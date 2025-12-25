#pragma once

#include "Helios/Resources/Image.h"
#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

namespace Helios
{
	class HELIOS_API Texture
    {
    public:
        virtual ~Texture() = default;
        
        virtual void SetData(void* data, uint32_t size) = 0;

        const uint32_t GetWidth() const { return m_image->GetSize().width; }
        const uint32_t GetHeight() const { return m_image->GetSize().height; }
        const Size GetSize() const { return m_image->GetSize(); }

        Ref<Image>& GetImage() { return m_image; }
        const Ref<Image>& GetImage() const { return m_image; }

        virtual void* GetTextureID() = 0;

		const UUID& GetID() const { return m_ID; }
        
        bool operator==(const Texture& other) const { return m_ID == other.m_ID; }
        bool operator!=(const Texture& other) const { return m_ID != other.m_ID; }
    private:
		UUID m_ID;
        Ref<Image> m_image;

		friend class Texture2D;
    };

    class GraphicsDevice;
    class HELIOS_API Texture2D final : public Texture
    {
    public:

        Texture2D(const Ref<GraphicsDevice>& device, const std::filesystem::path& path);
        Texture2D(const Ref<GraphicsDevice>& device, uint32_t width, uint32_t height);

        ~Texture2D() override = default;

        static Ref<Texture2D> Create(const std::filesystem::path& path);
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
        // static Ref<Texture2D> Create(std::vector<byte> data, uint32_t width, uint32_t height);

        void SetData(void* data, uint32_t size) override { m_image->SetData(data, size); }

        void* GetTextureID() override { return m_image->GetID(); }

    private:
        
    };
}