#include "Helios/Resources/Texture.h"
#include "pch.h"

namespace Helios
{
    class HELIOS_API Direct3D11Texture2D : public Texture2D
    {
    public:
        Direct3D11Texture2D(const std::filesystem::path& path);
        Direct3D11Texture2D(uint32_t width, uint32_t height);
        Direct3D11Texture2D(const Direct3D11Texture2D& other);
        //~DirectXTexture2D() override;

        void SetData(void* data, uint32_t size) override;

        void Bind(uint32_t slot = 0) override;
        void Unbind() override;

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }

        void* GetTextureID() const override { return m_TextureView.Get(); }

        bool operator==(const Texture& other) const override
        {
            return m_Texture == ((Direct3D11Texture2D&)other).m_Texture;
        }

        bool operator!=(const Texture& other) const override
        {
            return !(*this == other);
        }
    private:
        uint32_t m_Width, m_Height;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
        uint32_t m_lastBoundSlot = 0u;
    };
}