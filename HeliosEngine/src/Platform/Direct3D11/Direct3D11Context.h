#pragma once
#include "Helios/Core/Asserts.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/GraphicsContext.h"
#include "Helios/Graphics/Framebuffer.h"

namespace Helios
{
	class Direct3D11Context;
	class HELIOS_API D3D11MainRenderTarget : public Framebuffer
	{
	public:
		D3D11MainRenderTarget(const Direct3D11Context* context, Microsoft::WRL::ComPtr<IDXGIFactory> factory);
		~D3D11MainRenderTarget() { }

		void ClearBuffer(unsigned int bufferIndex, Color color) override;
		void ClearDepthStencil() override;

		void Invalidate() override;

		void Bind() override;

		void Unbind() override;

		void Bind(uint32_t attachment, uint32_t slot) override
		{

		}
		void Unbind(uint32_t attachment, uint32_t slot) override
		{

		}

		void BindDepth(uint32_t slot) override;
		void UnbindDepth(uint32_t slot) override;

		void Resize(uint32_t width, uint32_t height) override;

		void* GetTextureID(unsigned int bufferIndex) override { return m_RenderTargetView.Get(); }
		void* GetDepthTextureID() override { return m_depthStencilBuffer.shaderResourceView.Get(); }

		void CreateRenderTargetView();

		void SwapBuffers();

		// template GetPixel for uint and etc such as Color

		Color GetPixelColor(uint32_t attachment, uint32_t x, uint32_t y) override { return Color(0.0f, 0.0f, 0.0f, 0.0f); }
		Vector4Int GetPixelInt4(uint32_t attachment, uint32_t x, uint32_t y) override { return Vector4Int(0, 0, 0, 0); }

		Size GetSize() const override { return { m_Width, m_Height }; }
	private:
		const Direct3D11Context* m_Context;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

		struct
		{
			Format format;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		} m_depthStencilBuffer;

		friend class Direct3D11Context;
	};

	class Direct3D11Context : public GraphicsContext
	{
	public:
		Direct3D11Context(HWND windowHandle) : m_WindowHandle(windowHandle)
		{
			HL_ASSERT(m_WindowHandle, "HWND Window handle is null!");
		}

		void BindDefaultFramebuffer() override
		{
			m_MainRenderTarget->Bind();
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
					&s_Device, &s_featureLevel, &m_Context
				)
			), "Failed to create D3D11 Device!\n", hr);

			

			Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;
			s_Device.As(&dxgiDevice);

			Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
			Microsoft::WRL::ComPtr<IDXGIFactory> factory;

			HL_EXCEPTION_HR(FAILED(hr = dxgiDevice->GetAdapter(&adapter)), "Failed to get IDXGIAdapter!", hr);

			adapter->GetParent(IID_PPV_ARGS(&factory));



			//DXGI_SWAP_CHAIN_DESC swapChainDesc;
			//ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			//swapChainDesc.Windowed = TRUE;
			//swapChainDesc.BufferCount = 2;
			////swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			//swapChainDesc.SampleDesc.Count = 1;
			//swapChainDesc.SampleDesc.Quality = 0;
			//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			//swapChainDesc.OutputWindow = m_WindowHandle;
			//swapChainDesc.BufferDesc.Width = 0;
			//swapChainDesc.BufferDesc.Height = 0;
			//swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			//swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

			//HL_EXCEPTION_HR(FAILED(hr =
			//	factory->CreateSwapChain(m_Device.Get(), &swapChainDesc, &m_SwapChain)
			//), "Failed to create DXGI Swap Chain!", hr);

			//CreateRenderTargetView();

			m_MainRenderTarget = CreateRef<D3D11MainRenderTarget>(this, factory);

			UseContext();

			return true;
		}

		void SwapBuffers() override
		{
			std::static_pointer_cast<D3D11MainRenderTarget>(m_MainRenderTarget)->SwapBuffers();
			//m_MainRenderTarget->ClearBuffer(0u, Color::Red);
		}

		void SetVSync(uint32_t interval) override
		{
			m_SwapInterval = interval;
		}

		uint32_t GetVSyncInterval() override
		{
			return m_SwapInterval;
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
			return std::static_pointer_cast<D3D11MainRenderTarget>(m_MainRenderTarget)->m_SwapChain;
		}

		inline Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice()
		{
			return s_Device;
		}

		inline static Direct3D11Context* GetCurrentContext()
		{
			return s_CurrentContext;
		}

		void SetViewport(uint32_t width, uint32_t height) override
		{
			std::cout << "SetViewport: " << width << ", " << height << std::endl;
			m_MainRenderTarget->Resize(width, height);
		}

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const
		{
			return std::static_pointer_cast<D3D11MainRenderTarget>(m_MainRenderTarget)->m_RenderTargetView;
		}

		void BindMainRenderTargetView() const
		{
			m_MainRenderTarget->Bind();
		}

		Ref<Framebuffer> GetFramebuffer() override
		{
			return m_MainRenderTarget;
		}

	private:
		inline static Direct3D11Context* s_CurrentContext = nullptr;
		inline static Microsoft::WRL::ComPtr<ID3D11Device> s_Device;
		inline static D3D_FEATURE_LEVEL s_featureLevel;

		HWND m_WindowHandle;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;

		uint32_t m_SwapInterval = 1;

		Ref<Framebuffer> m_MainRenderTarget;

		friend class D3D11MainRenderTarget;
	};
}
