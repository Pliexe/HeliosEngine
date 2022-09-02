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

		ID3D10Device1*			m_d3Device			= NULL;
		IDXGISwapChain*			m_pSwapChain		= NULL;
		ID3D10RenderTargetView* m_d3RenderTarget	= NULL;
		
		ID2D1Factory* factory = NULL;
		ID2D1RenderTarget* renderTarget = NULL;
		IDWriteFactory* writeFactory = NULL;
		
		ID3D10ShaderResourceView1* pTextureView = nullptr;
		
		HWND m_hWnd;

		bool Init();

		/*void BeginDraw() { renderTarget->BeginDraw(); }
		void EndDraw() { renderTarget->EndDraw(); }*/

		bool CreateD3DDevice();
		bool Create3DRenderTarget();

		static Graphics* instance;

	public:
		Graphics() = delete;
		Graphics(HWND hWnd) : m_hWnd(hWnd) { instance = this; };
		~Graphics();

		void ClearRenderTarget(float r, float g, float b);
		void EndFrame();

		ID2D1RenderTarget* getRenderTarget() const { return renderTarget; }
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