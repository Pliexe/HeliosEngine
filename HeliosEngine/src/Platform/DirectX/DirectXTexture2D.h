#include "Helios/Resources/Texture.h"
#include "pch.h"

namespace Helios
{
    class DirectXTexture2D : public Texture2D
    {
    public:
        DirectXTexture2D(const std::string& path);
        // DirectXTexture2D(uint32_t width, uint32_t height);
        DirectXTexture2D(const DirectXTexture2D& other);
        //~DirectXTexture2D() override;


        void Bind(uint32_t slot = 0) const override;
        void Unbind() const override;

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }

        void* GetTextureID() const override { return m_TextureView.Get(); }

        bool operator==(const Texture& other) const override
        {
            return m_Texture == ((DirectXTexture2D&)other).m_Texture;
        }

        bool operator!=(const Texture& other) const override
        {
            return !(*this == other);
        }
    private:
        uint32_t m_Width, m_Height;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
    };
}