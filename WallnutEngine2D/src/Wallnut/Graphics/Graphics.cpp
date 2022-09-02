/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Graphics.h"
#include "Wallnut/Application.h"
#include "Wallnut/Utils/ErrorHandling.h"

using namespace Wallnut;

HRESULT Graphics::CreateD3D11Device(D3D_DRIVER_TYPE type)
{
    static const D3D_FEATURE_LEVEL levelAttempts[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    static UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

    this->m_device = NULL;

    return D3D11CreateDevice(
        NULL,
        type,
        NULL,
        flags,
        levelAttempts,
        ARRAYSIZE(levelAttempts),
        D3D11_SDK_VERSION,
        &this->m_device,
        NULL,
        &this->m_deviceContext
    );;
}

HRESULT Wallnut::Graphics::CreateD3D11SwapChain(IDXGIFactory* pFactory, IDXGISwapChain** ppSwapChain)
{
    DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));

    swapDesc.BufferDesc.Width = 0;
    swapDesc.BufferDesc.Height = 0;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = this->m_hWnd;
    swapDesc.Windowed = TRUE;
	
    return pFactory->CreateSwapChain(this->m_device, &swapDesc, ppSwapChain);
}

bool Wallnut::Graphics::Init()
{
    //if (!CreateD3DDevice()) return false;
    IDXGIAdapter* pAdapter10 = NULL;
    IDXGIDevice1* pDXGIDevice10 = NULL;
    IDXGIFactory* pDXGIFactory10 = NULL;

    IDXGIDevice* pDXGIDevice = NULL;
    IDXGIAdapter* pDXGIAdapter = NULL;
    IDXGIFactory* pDXGIFactory = NULL;

    HRESULT hr;

	// Create a new D3D11 device.
    if (FAILED(hr = CreateD3D11Device(D3D_DRIVER_TYPE_HARDWARE))) {
		if (FAILED(hr = CreateD3D11Device(D3D_DRIVER_TYPE_WARP))) {
			MessageBox(m_hWnd, (std::wstring(L"Failed to create Direct3D11 device. Reason: ") + GetErrorAsReadable(hr)).c_str(), L"Graphics Error!", MB_ICONERROR);
			return false;
		}
    }

    // Get DXGI Device for DX11
	hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	// Get DXGI Adapter for DX11
    hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
    // Get DXGI Factory for DX11
    pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);

	// Check HRESULT
	if (FAILED(hr)) {
		MessageBox(m_hWnd, (std::wstring(L"Failed to get DXGI device or adapter. Reason: ") + GetErrorAsReadable(hr)).c_str(), L"Graphics Error!", MB_ICONERROR);
		return false;
	}

	// Create Swap Chain for DX11
	if (FAILED(hr = CreateD3D11SwapChain(pDXGIFactory, &this->m_pSwapChain))) {
		MessageBox(m_hWnd, (std::wstring(L"Failed to create swap chain. Reason: ") + GetErrorAsReadable(hr)).c_str(), L"Graphics Error!", MB_ICONERROR);
		return false;
	}

	// Create Render Target for DX11
    if (FAILED(hr = CreateD3D11RenderTarget())) {
        MessageBox(m_hWnd, (std::wstring(L"Failed to create render target. Reason: ") + GetLastMessageAsReadable()).c_str(), L"Graphics Error!", MB_ICONERROR);
        return false;
    }

	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_MULTI_THREADED, &factory))) return false;

	RECT rect;
	GetClientRect(m_hWnd, &rect);

    IDXGISurface* pBackBuffer;

    // Create a Direct2D render target that can draw into the surface in the swap chain

    if (FAILED(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) return false;

    D2D1_RENDER_TARGET_PROPERTIES props =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

    if (FAILED(factory->CreateDxgiSurfaceRenderTarget(
        pBackBuffer,
        &props,
        /*D2D1::HwndRenderTargetProperties(
            m_hWnd, D2D1::SizeU(rect.right, rect.bottom)
        ),*/
        &m_d2renderTarget
    ))) {
		std::cout << "Failed to create render target: " << GetLastError() << std::endl;
        return false;
    }

    if (FAILED(hr = pBackBuffer->QueryInterface(&d2RenderTargetTexture))) {
        MessageBox(m_hWnd, (std::wstring(L"Failed to create dxgi texture. Reason: ") + GetLastMessageAsReadable()).c_str(), L"Graphics Error!", MB_ICONERROR);
        return false;
    }

    SafeRelease(&pBackBuffer);

	/*if (FAILED(factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			m_hWnd, D2D1::SizeU(rect.right, rect.bottom)
		),
		&m_d2renderTarget
	))) return false;*/

	if (FAILED(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(writeFactory),
		reinterpret_cast<IUnknown**>(&writeFactory)
	))) return false;

	return true;
}

void Wallnut::Graphics::EndFrame()
{
    m_pSwapChain->Present(1u, 0u);
    //m_pSwapChain_10->Present(1u, 0u);
}

void Wallnut::Graphics::ClearRenderTarget(float r, float g, float b)
{
    const float color[4] = { r,g,b,1.0f };
    this->m_deviceContext->ClearRenderTargetView(m_mainRenderTarget, color);
}

HRESULT Wallnut::Graphics::CreateD3D11RenderTarget()
{
	// Back Buffer Texture
	ID3D11Texture2D* pBB = nullptr;
    HRESULT hr;
    if (FAILED(hr = this->m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBB)))) return hr;
	if (FAILED(hr  = this->m_device->CreateRenderTargetView(pBB, NULL, &this->m_mainRenderTarget))) return hr;
    pBB->Release();
}

Wallnut::Graphics::~Graphics()
{
	SafeRelease(&factory);
	SafeRelease(&m_d2renderTarget);
	SafeRelease(&writeFactory);

    SafeRelease(&m_deviceContext);
    SafeRelease(&m_device);
    SafeRelease(&m_pSwapChain);
    SafeRelease(&d2RenderTargetTexture);

    SafeRelease(&pTextureView);
}

Wallnut::Graphics* Wallnut::Graphics::instance = nullptr;