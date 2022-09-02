/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Graphics.h"
#include "Wallnut/Application.h"

HRESULT CreateD3DDevice(
    IDXGIAdapter* pAdapter,
    D3D10_DRIVER_TYPE driverType,
    UINT flags,
    ID3D10Device1** ppDevice
)
{
    HRESULT hr = S_OK;

    static const D3D10_FEATURE_LEVEL1 levelAttempts[] =
    {
        D3D10_FEATURE_LEVEL_10_1,
        D3D10_FEATURE_LEVEL_10_0,
        D3D10_FEATURE_LEVEL_9_3,
        D3D10_FEATURE_LEVEL_9_2,
        D3D10_FEATURE_LEVEL_9_1,
    };

    for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
    {
        ID3D10Device1* pDevice = NULL;
        hr = D3D10CreateDevice1(
            pAdapter,
            driverType,
            NULL,
            flags,
            levelAttempts[level],
            D3D10_1_SDK_VERSION,
            &pDevice
        );

        if (SUCCEEDED(hr))
        {
            // transfer reference
            *ppDevice = pDevice;
            pDevice = NULL;
            break;
        }
    }

    return hr;
}

bool Wallnut::Graphics::Init()
{
    //if (!CreateD3DDevice()) return false;
    IDXGIAdapter* pAdapter = NULL;
    IDXGIDevice1* pDXGIDevice = NULL;
    IDXGIFactory* pDXGIFactory = NULL;

    HRESULT hr;

    // Create device
    hr = ::CreateD3DDevice(
        NULL,
        D3D10_DRIVER_TYPE_HARDWARE,
        D3D10_CREATE_DEVICE_BGRA_SUPPORT,
        &m_d3Device
    );

    if (FAILED(hr))
    {
        hr = ::CreateD3DDevice(
            NULL,
            D3D10_DRIVER_TYPE_WARP,
            D3D10_CREATE_DEVICE_BGRA_SUPPORT,
            &m_d3Device
        );
    }

    /*if (SUCCEEDED(hr))
    {
        hr = m_d3Device->QueryInterface(&m_d3Device);
    }*/
    if (SUCCEEDED(hr))
    {
        hr = m_d3Device->QueryInterface(&pDXGIDevice);
    }
    if (SUCCEEDED(hr))
    {
        hr = pDXGIDevice->GetAdapter(&pAdapter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));
    }
    if (SUCCEEDED(hr))
    {
        DXGI_SWAP_CHAIN_DESC swapDesc;
        ::ZeroMemory(&swapDesc, sizeof(swapDesc));

        swapDesc.BufferDesc.Width = 0;
        swapDesc.BufferDesc.Height = 0;
        swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swapDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapDesc.SampleDesc.Count = 1;
        swapDesc.SampleDesc.Quality = 0;
        swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.BufferCount = 1;
        swapDesc.OutputWindow = m_hWnd;
        swapDesc.Windowed = TRUE;

        hr = pDXGIFactory->CreateSwapChain(m_d3Device, &swapDesc, &m_pSwapChain);
    }
	
    //m_d3Device->QueryInterface(&pDXGIDevice);
    //pDXGIDevice->GetAdapter(&pAdapter);
    //pAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));

    if (!Create3DRenderTarget()) return false;

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

    //if (Create3DRenderTarget()) return false;

    if (FAILED(factory->CreateDxgiSurfaceRenderTarget(
        pBackBuffer,
        &props,
        /*D2D1::HwndRenderTargetProperties(
            m_hWnd, D2D1::SizeU(rect.right, rect.bottom)
        ),*/
        &renderTarget
    ))) {
		std::cout << "Failed to create render target: " << GetLastError() << std::endl;
        return false;
    }

    SafeRelease(&pBackBuffer);

	/*if (FAILED(factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			m_hWnd, D2D1::SizeU(rect.right, rect.bottom)
		),
		&renderTarget
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
}

void Wallnut::Graphics::ClearRenderTarget(float r, float g, float b)
{
    const float color[4] = { r,g,b,1.0f };
    m_d3Device->ClearRenderTargetView(m_d3RenderTarget, color);
}

bool Wallnut::Graphics::Create3DRenderTarget()
{
    ID3D10Texture2D* pBackBuffer;
    if (FAILED(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) return false;
    if (FAILED(m_d3Device->CreateRenderTargetView(pBackBuffer, NULL, &m_d3RenderTarget))) return false;
	pBackBuffer->Release();
    return true;
}

bool Wallnut::Graphics::CreateD3DDevice()
{
    DXGI_SWAP_CHAIN_DESC sd;
    ::ZeroMemory(&sd, sizeof(sd));

    sd.BufferDesc.Width = 200;
    sd.BufferDesc.Height = 200;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    //sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    //sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
    createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	
    /*if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &m_pSwapChain, &m_d3Device)))
        return false;*/

    Create3DRenderTarget();
    return true;

    //HRESULT hr = S_OK;

    //static const D3D10_FEATURE_LEVEL1 levelAttempts[] =
    //{
    //    D3D10_FEATURE_LEVEL_10_0,
    //    D3D10_FEATURE_LEVEL_9_3,
    //    D3D10_FEATURE_LEVEL_9_2,
    //    D3D10_FEATURE_LEVEL_9_1,
    //};

    //for (UINT level = 0; level < ARRAYSIZE(levelAttempts); level++)
    //{
    //    ID3D10Device1* pDevice = NULL;
    //    hr = D3D10CreateDevice1(
    //        pAdapter,
    //        driverType,
    //        NULL,
    //        flags,
    //        levelAttempts[level],
    //        D3D10_1_SDK_VERSION,
    //        &pDevice
    //    );

    //    if (SUCCEEDED(hr))
    //    {
    //        // transfer reference
    //        *ppDevice = pDevice;
    //        pDevice = NULL;
    //        break;
    //    }
    //}

    //return hr;
}

Wallnut::Graphics::~Graphics()
{
	SafeRelease(&factory);
	SafeRelease(&renderTarget);
	SafeRelease(&writeFactory);
    SafeRelease(&m_d3Device);
}

Wallnut::Graphics* Wallnut::Graphics::instance = nullptr;