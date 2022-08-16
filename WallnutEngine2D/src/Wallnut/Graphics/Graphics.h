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

		ID2D1Factory* factory = NULL;
		ID2D1HwndRenderTarget* renderTarget = NULL;
		IDWriteFactory* writeFactory = NULL;

		HWND m_hWnd;

		bool Init();

		/*void BeginDraw() { renderTarget->BeginDraw(); }
		void EndDraw() { renderTarget->EndDraw(); }*/

		static Graphics* instance;

	public:
		Graphics() = delete;
		Graphics(HWND hWnd) : m_hWnd(hWnd) { instance = this; };
		~Graphics();

		ID2D1HwndRenderTarget* getRenderTarget() const { return renderTarget; }
		ID2D1Factory* getFactory() const { return factory; }
		IDWriteFactory* getWriteFactory() const { return writeFactory; }

		friend class Application;
		friend class SceneManager;
	};
}