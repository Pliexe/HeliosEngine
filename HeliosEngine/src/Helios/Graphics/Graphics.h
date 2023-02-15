/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Translation/Vector.h"

namespace Helios {
	class HELIOS_API Graphics
	{
	public:

		enum class API
		{
			None = 0,
			DirectX = 1,
			OpenGL = 2,
		};

		static API GetAPI() { return s_API; }

		static Size GetCurrentSize() { return s_currentSize; }

	private:

		static Size s_currentSize;

		static API s_API;

		// TODO: Cleanup this mess

		IDXGISwapChain*			m_pSwapChain			= NULL;
		
		ID3D11DeviceContext*	m_deviceContext			= NULL;
		ID3D11Device*			m_device  				= NULL;
		
		ID3D11RenderTargetView*	m_mainRenderTarget		= NULL;
		ID3D11RenderTargetView* m_sceneRenderTarget		= NULL;
		ID3D11RenderTargetView* m_gameRenderTarget		= NULL;
		
		ID3D11Texture2D * d2RenderTargetTexture			= NULL;
		
		ID2D1Factory*			factory					= nullptr;
		ID2D1RenderTarget*		m_renderTarget2D		= nullptr;
		IDWriteFactory*			writeFactory			= nullptr;
		ID3D11Texture2D*		m_renderTarget2Dtexture = nullptr;
		ID3D11Texture2D* m_renderTarget2DtextureTest = nullptr;		
		
		ID3D11ShaderResourceView* pTextureView = nullptr;
		ID3D11ShaderResourceView* pTextureViewtest = nullptr;
		ID3D11Texture2D* pRenderTexture = nullptr;
		ID3D11RenderTargetView* pRenderTargetRef = nullptr;
		
		HWND m_hWnd;

		bool Init();

		/*void BeginDraw() { m_renderTarget2D->BeginDraw(); }
		void EndDraw() { m_renderTarget2D->EndDraw(); }*/

		HRESULT CreateD3D11Device(D3D_DRIVER_TYPE type);
		HRESULT CreateD3D11SwapChain(IDXGIFactory* pFactory, IDXGISwapChain** ppSwapChain);
		HRESULT CreateD3D11RenderTarget();

		HRESULT CreateDXGIRenderTarget(UINT width, UINT height);

		static Graphics* instance;

	public:
		Graphics() = delete;
		Graphics(HWND hWnd) : m_hWnd(hWnd) { instance = this; s_API = API::DirectX; };
		~Graphics();

		void ClearRenderTarget(float r, float g, float b);
		void EndFrame();

		ID2D1RenderTarget* getRenderTarget() const { return m_renderTarget2D; }
		ID2D1Factory* getFactory() const { return factory; }
		IDWriteFactory* getWriteFactory() const { return writeFactory; }
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterizerState;

		friend class Application;
		friend class SceneManager;
		friend class Scene;

		friend class Renderer;
		friend class Renderer2D;
		friend class GizmosRenderer;
		friend class UIRenderer;

#pragma region Resources

		friend class Bitmap;
		friend class SolidBrush;
		friend class LinearGradientBrush;
		friend class RadialGradientBrush;
		friend class BitmapBrush;

		friend class Shader;
		friend class IndexBuffer;
		friend class DirectXVertexBuffer;
		friend class DirectXConstantBuffer;
		friend class DirectXTexture2D;
		friend class DirectXMaterial;
		
#pragma endregion

		friend class DirectXFramebuffer;

		friend class EngineTexture;
		extern friend class GameEngine;
	};
}