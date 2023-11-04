#pragma once
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/GraphicsContext.h"

namespace Helios
{
	class Direct3D11Context : public GraphicsContext
	{
	public:
		Direct3D11Context(HWND windowHandle) : m_WindowHandle(windowHandle)
		{
			HL_CORE_ASSERT_WITH_MSG(m_WindowHandle, "HWND Window handle is null!");
		}

		void BindDefaultFramebuffer() override
		{
			m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
		}

		bool Init() override
		{
			HRESULT hr;

			D3D_FEATURE_LEVEL levels[] = {
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0
			};

			UINT flags = 0;

#ifdef HELIOS_DEBUG
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			HL_EXCEPTION_HR(FAILED(
				hr = D3D11CreateDevice(
					nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
					levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
					&m_Device, &m_featureLevel, &m_Context
				)
			), "Failed to create D3D11 Device!\n", hr);

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
			swapChainDesc.OutputWindow = m_WindowHandle;
			swapChainDesc.BufferDesc.Width = 0;
			swapChainDesc.BufferDesc.Height = 0;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

			Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
			m_Device.As(&dxgiDevice);

			Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
			Microsoft::WRL::ComPtr<IDXGIFactory> factory;

			HL_EXCEPTION_HR(FAILED(hr = dxgiDevice->GetAdapter(&adapter)), "Failed to get IDXGIAdapter!", hr);

			adapter->GetParent(IID_PPV_ARGS(&factory));

			HL_EXCEPTION_HR(FAILED(hr =
				factory->CreateSwapChain(m_Device.Get(), &swapChainDesc, &m_SwapChain)
			), "Failed to create DXGI Swap Chain!", hr);

			CreateRenderTargetView();

			UseContext();

			return true;
		}

		void SwapBuffers() override
		{
			m_SwapChain->Present(m_SwapInterval, 0);
			//float c[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
			//m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), c);
		}

		void SetVSync(uint32_t interval) override
		{
			m_SwapInterval = interval;
		}

		void UseContext() override
		{
			s_CurrentContext = this;
		}

		inline Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext()
		{
			return m_Context;
		}

		inline Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain()
		{
			return m_SwapChain;
		}

		inline Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice()
		{
			return m_Device;
		}

		inline static Direct3D11Context* GetCurrentContext()
		{
			return s_CurrentContext;
		}

		void SetViewport(uint32_t width, uint32_t height) override
		{
			std::cout << "SetViewport: " << width << ", " << height << std::endl;
			m_RenderTargetView.Reset();
			m_SwapChain->ResizeBuffers( 2, width, height, DXGI_FORMAT_UNKNOWN, 0 );
			CreateRenderTargetView();
		}

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const
		{
			return m_RenderTargetView;
		}

		void BindMainRenderTargetView() const
		{
			m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
		}

	private:

		void CreateRenderTargetView()
		{
			HRESULT hr;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
			HL_EXCEPTION_HR(FAILED(hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))), "Failed to get back buffer!", hr);
			HL_EXCEPTION_HR(FAILED(hr = m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_RenderTargetView)), "Failed to create render target view!", hr);
		}

	private:
		inline static Direct3D11Context* s_CurrentContext = nullptr;

		HWND m_WindowHandle;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

		D3D_FEATURE_LEVEL m_featureLevel;
		uint32_t m_SwapInterval = 1;
	};
}
