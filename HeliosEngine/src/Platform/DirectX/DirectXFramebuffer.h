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
        DirectXFramebuffer(uint32_t width, uint32_t height, Format format);
        ~DirectXFramebuffer();
		
        void ClearColor(Color color);

        void Invalidate() override;

        void Bind() override;
        void Unbind() override;

        void AddDepthStencil() override;
        void RemoveDepthStencil() override;

        virtual bool IsSet() override;
		
        void Resize(uint32_t width, uint32_t height) override;

        void* GetTextureID() override;
        Size GetSize() const override;

        struct FormatUInt2
        {
			uint32_t r;
			uint32_t g;
        };
		
		// std::enable_if function if m_Format is Format::Uint2
		template <Format T>
        typename std::enable_if<T == Format::Uint2, FormatUInt2>::type
			GetPixel(uint32_t attachment, uint32_t x, uint32_t y)
		{
			// Read pixel from m_Texture
			FormatUInt2 data;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			

			
			//Graphics::instance->m_deviceContext->Map(m_Texture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
			/*FormatUInt2* pixels = (FormatUInt2*)mappedResource.pData;
			data = pixels[y * m_Width + x];
			Graphics::instance->m_deviceContext->Unmap(m_Texture.Get(), 0);
			*/return data;
        }
		
    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
        // Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

        uint32_t m_Width;
        uint32_t m_Height;
    };
}