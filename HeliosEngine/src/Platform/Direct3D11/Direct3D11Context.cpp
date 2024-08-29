#include "Direct3D11Context.h"

inline Helios::D3D11MainRenderTarget::D3D11MainRenderTarget(const Direct3D11Context* context, Microsoft::WRL::ComPtr<IDXGIFactory> factory) : m_Context(context)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = m_Context->m_WindowHandle;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	HL_EXCEPTION_HR(FAILED(hr =
		factory->CreateSwapChain(m_Context->s_Device.Get(), &swapChainDesc, &m_SwapChain)
	), "Failed to create DXGI Swap Chain!", hr);

	CreateRenderTargetView();
}

inline void Helios::D3D11MainRenderTarget::ClearBuffer(unsigned int bufferIndex, Color color) { m_Context->m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color); }

inline void Helios::D3D11MainRenderTarget::ClearDepthStencil() { m_Context->m_Context->ClearDepthStencilView(m_depthStencilBuffer.depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0); }

inline void Helios::D3D11MainRenderTarget::Invalidate()
{
	m_RenderTargetView.Reset();
	m_SwapChain->ResizeBuffers(2, m_Width, m_Height, DXGI_FORMAT_UNKNOWN, 0);
	CreateRenderTargetView();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	HL_EXCEPTION(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilBuffer.depthStencilState),
		"Failed to create depth stencil state");

	D3D11_TEXTURE2D_DESC descTexture;
	ZeroMemory(&descTexture, sizeof(descTexture));
	descTexture.Width = (UINT)m_Width;
	descTexture.Height = (UINT)m_Height;
	descTexture.MipLevels = 1u;
	descTexture.ArraySize = 1u;
	descTexture.Format = DXGI_FORMAT_R32_TYPELESS;
	descTexture.SampleDesc.Count = 1u;
	descTexture.SampleDesc.Quality = 0u;
	descTexture.Usage = D3D11_USAGE_DEFAULT;
	descTexture.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	HL_EXCEPTION(FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateTexture2D(&descTexture, nullptr, &m_depthStencilBuffer.texture)),
		"Failed to create depth stencil buffer");

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	HL_EXCEPTION(FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateDepthStencilView(m_depthStencilBuffer.texture.Get(), &descDSV, &m_depthStencilBuffer.depthStencilView)),
		"Failed to create depth stencil view");

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
	descSRV.Format = DXGI_FORMAT_R32_FLOAT;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = 1u;

	HL_EXCEPTION(FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateShaderResourceView(m_depthStencilBuffer.texture.Get(), &descSRV, &m_depthStencilBuffer.shaderResourceView)),
		"Failed to create depth stencil shader resource view");
}

inline void Helios::D3D11MainRenderTarget::Bind()
{
	m_Context->m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_depthStencilBuffer.depthStencilView.Get());

	//Direct3D11Context::GetCurrentContext()->GetContext()->OMSetRenderTargets(m_colorBuffers.size(), m_renderTargetViews.data(), m_depthStencilBuffer.depthStencilView.Get());

	if (m_depthStencilBuffer.depthStencilState)
		Direct3D11Context::GetCurrentContext()->GetContext()->OMSetDepthStencilState(m_depthStencilBuffer.depthStencilState.Get(), 1u);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)m_Width;
	viewport.Height = (float)m_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	Direct3D11Context::GetCurrentContext()->GetContext()->RSSetViewports(1, &viewport);
}

inline void Helios::D3D11MainRenderTarget::Unbind()
{
	Direct3D11Context::GetCurrentContext()->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	Direct3D11Context::GetCurrentContext()->GetContext()->OMSetDepthStencilState(nullptr, 1);
}

inline void Helios::D3D11MainRenderTarget::BindDepth(uint32_t slot)
{
	Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShaderResources(slot, 1, m_depthStencilBuffer.shaderResourceView.GetAddressOf());
}

inline void Helios::D3D11MainRenderTarget::UnbindDepth(uint32_t slot)
{
	ID3D11ShaderResourceView* nullsrv[] = { nullptr };
	Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShaderResources(slot, 1, nullsrv);
}

inline void Helios::D3D11MainRenderTarget::Resize(uint32_t width, uint32_t height)
{
	HL_ASSERT(width > 0 && height > 0 && width <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION && height <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION, "Invalid framebuffer size");
	m_Width = width;
	m_Height = height;
	Invalidate();
}

inline void Helios::D3D11MainRenderTarget::CreateRenderTargetView()
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HL_EXCEPTION_HR(FAILED(hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))), "Failed to get back buffer!", hr);
	HL_EXCEPTION_HR(FAILED(hr = m_Context->s_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_RenderTargetView)), "Failed to create render target view!", hr);
}

inline void Helios::D3D11MainRenderTarget::SwapBuffers()
{
	m_SwapChain->Present(m_Context->m_SwapInterval, 0);
	float c[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_Context->m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), c);
	if(m_depthStencilBuffer.depthStencilView) ClearDepthStencil();
}
