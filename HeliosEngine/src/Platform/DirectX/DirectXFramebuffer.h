#pragma once

#include "Helios/Graphics/Framebuffer.h"
#include "pch.h"
#include <wrl.h>
#include <Helios/Resources/Color.h>

namespace Helios
{
    class DirectXFramebuffer : public Framebuffer
    {
    public:
        DirectXFramebuffer(uint32_t width, uint32_t height);
        ~DirectXFramebuffer();
		
        void ClearColor(Color color);

        void Invalidate() override;

        void Bind() override;
        void Unbind() override;

        virtual bool IsSet() override;
		
        void Resize(uint32_t width, uint32_t height) override;

        void* GetTextureID() override;
        Size GetSize() const override;
    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
        // Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;

        uint32_t m_Width;
        uint32_t m_Height;
    };
}