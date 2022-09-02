/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class WALLNUT_API Graphics
	{
	private:
		IDXGISwapChain*			m_pSwapChain			= NULL;
		
		ID3D11DeviceContext*	m_deviceContext			= NULL;
		ID3D11Device*			m_device  				= NULL;
		
		ID3D11RenderTargetView*	m_mainRenderTarget		= NULL;
		ID3D11RenderTargetView* m_sceneRenderTarget		= NULL;
		ID3D11RenderTargetView* m_gameRenderTarget		= NULL;
		
		ID3D11Texture2D * d2RenderTargetTexture			= NULL;
		
		ID2D1Factory* factory							= NULL;
		ID2D1RenderTarget* m_d2renderTarget				= NULL;
		IDWriteFactory* writeFactory					= NULL;
		
		ID3D11ShaderResourceView* pTextureView = nullptr;
		ID3D11Texture2D* pRenderTexture = nullptr;
		ID3D11RenderTargetView* pRenderTargetRef = nullptr;
		
		HWND m_hWnd;

		bool Init();

		/*void BeginDraw() { m_d2renderTarget->BeginDraw(); }
		void EndDraw() { m_d2renderTarget->EndDraw(); }*/

		HRESULT CreateD3D11Device(D3D_DRIVER_TYPE type);
		HRESULT CreateD3D11SwapChain(IDXGIFactory* pFactory, IDXGISwapChain** ppSwapChain);
		HRESULT CreateD3D11RenderTarget();

		static Graphics* instance;

	public:
		Graphics() = delete;
		Graphics(HWND hWnd) : m_hWnd(hWnd) { instance = this; };
		~Graphics();

		void ClearRenderTarget(float r, float g, float b);
		void EndFrame();

		ID2D1RenderTarget* getRenderTarget() const { return m_d2renderTarget; }
		ID2D1Factory* getFactory() const { return factory; }
		IDWriteFactory* getWriteFactory() const { return writeFactory; }

		friend class Application;
		friend class SceneManager;

#pragma region Resources

		friend class Bitmap;
		friend class SolidBrush;
		friend class LinearGradientBrush;
		friend class RadialGradientBrush;
		friend class BitmapBrush;

#pragma endregion

		extern friend class GameEngine;
	};
}