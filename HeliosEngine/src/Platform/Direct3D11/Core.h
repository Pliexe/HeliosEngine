#pragma once

#ifdef HELIOS_PLATFORM_WINDOWS

#include "Platform/Windows/Win32GraphicalWindow.h"
#include "pch.h"

namespace Helios::D3D11
{
	static Microsoft::WRL::ComPtr<ID3D11Device> s_device = nullptr;
	static Microsoft::WRL::ComPtr<ID3D11DeviceContext >_deviceContext = nullptr;
	static Microsoft::WRL::ComPtr<IDXGISwapChain> s_swapChain = nullptr;
	static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;

	void InitializeD3D11()
	{

	}

	void CreateD3D11Device()
	{
		static const D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		static uint32_t flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef HELIOS_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		return D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&s_device,
			nullptr,
			nullptr
		);
	}

	void CreateD3D11Factory()
	{
		CreateDXGIFactory2(0, IID_PPV_ARGS(&s_factory));
	}
}

#endif