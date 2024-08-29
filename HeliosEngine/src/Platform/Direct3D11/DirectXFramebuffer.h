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
        
        DirectXFramebuffer(uint32_t width, uint32_t height, std::initializer_list<Format> bufferSpecifications);
        ~DirectXFramebuffer();

		void ClearBuffer(unsigned int bufferIndex, Color color) override;
		void ClearDepthStencil() override;

        void Invalidate() override;

        void Bind() override;
        void Unbind() override;

        void Bind(uint32_t attachment, uint32_t slot) override;
        void Unbind(uint32_t attachment, uint32_t slot) override;

        void BindDepth(uint32_t slot) override;
        void UnbindDepth(uint32_t slot) override;
		
        void Resize(uint32_t width, uint32_t height) override;

        void* GetTextureID(unsigned int bufferIndex) override;
        void* GetDepthTextureID() override;


        // template GetPixel for uint and etc such as Color



		Color GetPixelColor(uint32_t attachment, uint32_t x, uint32_t y) override;
        Vector4Int GetPixelInt4(uint32_t attachment, uint32_t x, uint32_t y) override;
   
        Size GetSize() const override;

        struct FormatUInt2
        {
			uint32_t r;
			uint32_t g;
        };
		
		//// std::enable_if function if m_Format is Format::Uint2
		//template <Format T>
  //      typename std::enable_if<T == Format::Uint2, FormatUInt2>::type
		//	GetPixel(uint32_t attachment, uint32_t x, uint32_t y)
		//{
		//	// Read pixel from m_Texture
		//	FormatUInt2 data;
		//	D3D11_MAPPED_SUBRESOURCE mappedResource;
		//	

		//	
		//	//Direct3D11Context::GetCurrentContext()->GetContext()->Map(m_Texture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
		//	/*FormatUInt2* pixels = (FormatUInt2*)mappedResource.pData;
		//	data = pixels[y * m_Width + x];
		//	Direct3D11Context::GetCurrentContext()->GetContext()->Unmap(m_Texture.Get(), 0);
		//	*/return data;
  //      }
		
    private:

        struct BufferSpecification
        {
            Format format;
            Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
        };

        std::vector<BufferSpecification> m_colorBuffers;
        std::vector<ID3D11RenderTargetView*> m_renderTargetViews;
        struct 
        {
            Format format;
            Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
        } m_depthStencilBuffer;
    };
}