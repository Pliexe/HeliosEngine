#include "DirectXFramebuffer.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
    DirectXFramebuffer::DirectXFramebuffer(uint32_t width, uint32_t height, Format format)
        : m_Width(width), m_Height(height)
    {
		m_Format = format;
        Invalidate();
    }

    DirectXFramebuffer::~DirectXFramebuffer()
    {
        // No need to Release() the ComPtrs, they will be released automatically
    }
	
    void DirectXFramebuffer::ClearColor(Color color)
    {
        Graphics::instance->m_deviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color.c);
        if(m_depthStencilView) 
            Graphics::instance->m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void* DirectXFramebuffer::GetTextureID()
    {
        return (void*)m_ShaderResourceView.Get();
    }

    Size DirectXFramebuffer::GetSize() const
    {
        return { (float)m_Width, (float)m_Height };
    }

    void DirectXFramebuffer::Invalidate()
    {
        // Create the texture
        D3D11_TEXTURE2D_DESC textureDesc;
        
        ZeroMemory(&textureDesc, sizeof(textureDesc));

        textureDesc.Width = (UINT)m_Width;
        textureDesc.Height = (UINT)m_Height;
        textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        switch (m_Format)
        {
		case Format::Float4:
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case Format::Uint2:
			textureDesc.Format = DXGI_FORMAT_R32G32_UINT;
			break;
        }

		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

        HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateTexture2D(&textureDesc, nullptr, &m_Texture)),
            "Failed to create texture for framebuffer");

        // Create the render target view

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateRenderTargetView(m_Texture.Get(), &renderTargetViewDesc, &m_RenderTargetView)),
            "Failed to create render target view for framebuffer");

        // Create the shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateShaderResourceView(m_Texture.Get(), &shaderResourceViewDesc, &m_ShaderResourceView)),
            "Failed to create shader resource view for framebuffer");

        // TODO: Create depth stencil view

		// Set the render target view
		Graphics::instance->m_deviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
		
		// Set the viewport
		D3D11_VIEWPORT viewport = {};
		viewport.Width = (float)m_Width;
		viewport.Height = (float)m_Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		Graphics::instance->m_deviceContext->RSSetViewports(1, &viewport);

        if (m_depthStencilView) {
            RemoveDepthStencil();
            AddDepthStencil();
        }
    }

    void DirectXFramebuffer::Bind()
    {
        Graphics::s_currentSize = { (float)m_Width, (float)m_Height };
        Graphics::instance->m_deviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStencilView.Get());
		if(m_depthStencilView)
		    Graphics::instance->m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1u);

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
	    if(m_depthStencilView)
			Graphics::instance->m_deviceContext->OMSetDepthStencilState(nullptr, 1);
    }

    void DirectXFramebuffer::AddDepthStencil()
    {
        if (m_depthStencilView == nullptr)
        {
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

			HL_EXCEPTION(Graphics::instance->m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState),
				"Failed to create depth stencil state");
			
			D3D11_TEXTURE2D_DESC descDepth = {};
			descDepth.Width = (UINT)m_Width;
			descDepth.Height = (UINT)m_Height;
			descDepth.MipLevels = 1u;
			descDepth.ArraySize = 1u;
			descDepth.Format = DXGI_FORMAT_D32_FLOAT;
			descDepth.SampleDesc.Count = 1u;
			descDepth.SampleDesc.Quality = 0u;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

            HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateTexture2D(&descDepth, nullptr, &m_depthStencilBuffer)),
                "Failed to create depth stencil buffer");

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0u;

            HL_EXCEPTION(FAILED(Graphics::instance->m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &descDSV, m_depthStencilView.GetAddressOf())),
                "Failed to create depth stencil view");
        }
    }

    void DirectXFramebuffer::RemoveDepthStencil()
    {
        m_depthStencilView.Reset();
        m_depthStencilBuffer.Reset();
    }

    void DirectXFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        Invalidate();
    }

    bool DirectXFramebuffer::IsSet() { return m_ShaderResourceView != nullptr;}
}