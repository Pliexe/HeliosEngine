#include "DirectXFramebuffer.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
	static DXGI_FORMAT GetDXGIFormat(Framebuffer::Format format)
	{
		switch (format)
		{
		case Framebuffer::Format::R32G32B32A32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Framebuffer::Format::R32G32B32F: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Framebuffer::Format::R32G32F: return DXGI_FORMAT_R32G32_FLOAT;
		case Framebuffer::Format::R32F: return DXGI_FORMAT_R32_FLOAT;
		case Framebuffer::Format::R8B8G8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Framebuffer::Format::D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case Framebuffer::Format::D32F: return DXGI_FORMAT_D32_FLOAT;
		default:
			HL_ASSERT_EXCEPTION(false, "Unknown format!");
			return DXGI_FORMAT_UNKNOWN;
		}
	}

    DirectXFramebuffer::DirectXFramebuffer(uint32_t width, uint32_t height, std::initializer_list<Format> bufferSpecifications)
    {
		m_Width = width;
		m_Height = height;
		
        for (auto& format : bufferSpecifications)
        {
            switch (format)
            {
			case Format::NONE:
				break;
            case Format::D24S8:
			case Format::D32F:
				m_depthStencilBuffer.format = format;
				break;
			default:
				m_colorBuffers.emplace_back(BufferSpecification{ format });
				m_renderTargetViews.emplace_back(nullptr);
				break;
			}
	    }
        Invalidate();
    }

    DirectXFramebuffer::~DirectXFramebuffer()
    {
		for (auto& renderTargets : m_renderTargetViews)
			SafeRelease(&renderTargets);
		
        // No need to Release() the ComPtrs, they will be released automatically
    }

	void DirectXFramebuffer::ClearBuffer(unsigned int bufferIndex, Color color)
	{
		//HL_ASSERT_EXCEPTION(bufferIndex < m_colorBuffers.size(), "Invalid buffer index!");
		auto& test = m_colorBuffers[bufferIndex];
		auto test2 = m_colorBuffers.size();
		Graphics::instance->m_deviceContext->ClearRenderTargetView(m_renderTargetViews[bufferIndex], color.c);
	}

	void DirectXFramebuffer::ClearDepthStencil()
	{
		HL_ASSERT_EXCEPTION(m_depthStencilBuffer.depthStencilView, "Depth stencil buffer not set!");
		Graphics::instance->m_deviceContext->ClearDepthStencilView(m_depthStencilBuffer.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

    void* DirectXFramebuffer::GetTextureID(unsigned int bufferIndex)
    {
		//HL_ASSERT_EXCEPTION(bufferIndex < m_colorBuffers.size(), std::string("Invalid buffer index!") + " " + std::to_string(bufferIndex) + " < " + std::to_string(m_colorBuffers.size()));
		auto test = m_colorBuffers[bufferIndex];
		return m_colorBuffers[bufferIndex].shaderResourceView.Get();
    }

	Color DirectXFramebuffer::GetPixel(uint32_t attachment, uint32_t x, uint32_t y)
	{
		// Assert if out of bounds
		HL_ASSERT_EXCEPTION((x < m_Width&& x >= 0), "Error while reading Pixel data! (X coordinate out of bounds!)");
		HL_ASSERT_EXCEPTION((y < m_Height&& y >= 0), "Error while reading Pixel data! (Y coordinate out of bounds!)");

		// Copy the	render target to a staging texture so we can read it back from the CPU
		Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
		D3D11_TEXTURE2D_DESC stagingTextureDesc;
		ZeroMemory(&stagingTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		stagingTextureDesc.Width = m_Width;
		stagingTextureDesc.Height = m_Height;
		stagingTextureDesc.MipLevels = 1;
		stagingTextureDesc.ArraySize = 1;
		stagingTextureDesc.Format = GetDXGIFormat(m_colorBuffers[attachment].format);
		stagingTextureDesc.SampleDesc.Count = 1;
		stagingTextureDesc.SampleDesc.Quality = 0;
		stagingTextureDesc.Usage = D3D11_USAGE_STAGING;
		stagingTextureDesc.BindFlags = 0;
		stagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingTextureDesc.MiscFlags = 0;
		
		HRESULT hr;

		hr = Graphics::instance->m_device->CreateTexture2D(&stagingTextureDesc, nullptr, stagingTexture.GetAddressOf());

		HL_EXCEPTION_HR(FAILED(hr), "Failed to create staging texture!", hr);

		Graphics::instance->m_deviceContext->CopyResource(stagingTexture.Get(), m_colorBuffers[attachment].texture.Get());

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = Graphics::instance->m_deviceContext->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
		
		HL_EXCEPTION_HR(FAILED(hr), "Failed to map staging texture!", hr);

		Color color;

		switch (m_colorBuffers[attachment].format)
		{
		case Format::R32G32B32A32F:
			color = *(Color*)(((uint8_t*)mappedResource.pData) + (y * mappedResource.RowPitch) + (x * sizeof(Color)));
			//// Print out the resource
			//for (int i = 0; i < m_Width * m_Height; i++)
			//{
			//	Color* color = (Color*)(((uint8_t*)mappedResource.pData) + (i * sizeof(Color)));
			//	// print new line per row
			//	if (i % m_Width == 0)
			//		std::cout << std::endl;
			//	std::cout << "(" << color->r << ", " << color->g << ", " << color->b << ", " << color->a << ") ";
			//}
			break;
		case Format::R8B8G8A8_UNORM:
			color = *(Color*)(((uint8_t*)mappedResource.pData) + (y * mappedResource.RowPitch) + (x * sizeof(Color)));
			break;
		case Format::R32F:
			color = Color(*(float*)(((uint8_t*)mappedResource.pData) + (y * mappedResource.RowPitch) + (x * sizeof(float))), 0.0f, 0.0f, 1.0f);
			break;
		/*case Format::R32G32B32A32F:
		{
			float* data = (float*)mappedResource.pData;
			color.r = data[4 * x + y * m_Width * 4];
			color.g = data[4 * x + y * m_Width * 4 + 1];
			color.b = data[4 * x + y * m_Width * 4 + 2];
			color.a = data[4 * x + y * m_Width * 4 + 3];
			break;
		}
		case Format::R8B8G8A8_UNORM:
		{
			uint8_t* data = (uint8_t*)mappedResource.pData;
			color.r = data[4 * x + y * m_Width * 4] / 255.0f;
			color.g = data[4 * x + y * m_Width * 4 + 1] / 255.0f;
			color.b = data[4 * x + y * m_Width * 4 + 2] / 255.0f;
			color.a = data[4 * x + y * m_Width * 4 + 3] / 255.0f;
			break;
		}
		case Format::R32F:
		{
			float* data = (float*)mappedResource.pData;
			color.r = data[x + y * m_Width];
			color.g = data[x + y * m_Width];
			color.b = data[x + y * m_Width];
			color.a = data[x + y * m_Width];
			break;
		}*/
		default:
			HL_ASSERT_EXCEPTION(false, "Unknown format!");
			break;
		}
		
		Graphics::instance->m_deviceContext->Unmap(stagingTexture.Get(), 0);

		return color;
	}

    Size DirectXFramebuffer::GetSize() const
    {
        return { (float)m_Width, (float)m_Height };
    }

    

    void DirectXFramebuffer::Invalidate()
    {
		int i = 0;
		for (auto& colorBuffer : m_colorBuffers)
		{
			// Create texture
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = m_Width;
			textureDesc.Height = m_Height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Format = GetDXGIFormat(colorBuffer.format);
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0u;
			textureDesc.MiscFlags = 0;

			HRESULT hr;
            HL_EXCEPTION_HR(FAILED(hr = Graphics::instance->m_device->CreateTexture2D(&textureDesc, nullptr, &colorBuffer.texture)),
                "Failed to create texture for framebuffer", hr);

			// Create render target view
			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			SafeRelease(&m_renderTargetViews[i]);
			HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateRenderTargetView(colorBuffer.texture.Get(), &renderTargetViewDesc, &m_renderTargetViews[i])),
				"Failed to create render target view for framebuffer");

			// Create shader resource view
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
			shaderResourceViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateShaderResourceView(colorBuffer.texture.Get(), &shaderResourceViewDesc, &colorBuffer.shaderResourceView)),
				"Failed to create shader resource view for framebuffer");

			i++;
		}

		// Create depth stencil buffer if specified

		if (m_depthStencilBuffer.format != Format::NONE)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

			HL_EXCEPTION(Graphics::instance->m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilBuffer.depthStencilState),
				"Failed to create depth stencil state");

			D3D11_TEXTURE2D_DESC descTexture;
			ZeroMemory(&descTexture, sizeof(descTexture));
			descTexture.Width = (UINT)m_Width;
			descTexture.Height = (UINT)m_Height;
			descTexture.MipLevels = 1u;
			descTexture.ArraySize = 1u;
			descTexture.Format = DXGI_FORMAT_D32_FLOAT;
			descTexture.SampleDesc.Count = 1u;
			descTexture.SampleDesc.Quality = 0u;
			descTexture.Usage = D3D11_USAGE_DEFAULT;
			descTexture.BindFlags = D3D11_BIND_DEPTH_STENCIL;

			HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateTexture2D(&descTexture, nullptr, &m_depthStencilBuffer.texture)),
				"Failed to create depth stencil buffer");

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
			descDSV.Format = descTexture.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0u;

			HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateDepthStencilView(m_depthStencilBuffer.texture.Get(), &descDSV, &m_depthStencilBuffer.depthStencilView)),
				"Failed to create depth stencil view");
		}

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
		viewport.Width = (float)m_Width;
		viewport.Height = (float)m_Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		
		Graphics::instance->m_deviceContext->RSSetViewports(1, &viewport);
    }

    void DirectXFramebuffer::Bind()
    {
        Graphics::s_currentSize = { (float)m_Width, (float)m_Height };
		Graphics::instance->m_deviceContext->OMSetRenderTargets(m_colorBuffers.size(), m_renderTargetViews.data(), m_depthStencilBuffer.depthStencilView.Get());

		if (m_depthStencilBuffer.depthStencilState)
			Graphics::instance->m_deviceContext->OMSetDepthStencilState(m_depthStencilBuffer.depthStencilState.Get(), 1u);

        D3D11_VIEWPORT viewport = {};
        viewport.Width = (float)m_Width;
        viewport.Height = (float)m_Height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        Graphics::instance->m_deviceContext->RSSetViewports(1, &viewport);
    }

    void DirectXFramebuffer::Unbind()
    {
        Graphics::instance->m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		Graphics::instance->m_deviceContext->OMSetDepthStencilState(nullptr, 1);
    }

    void DirectXFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        Invalidate();
    }
}