/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Core.h"
#include "Graphics/Graphics.h"
#include "Scene/SceneManager.h"

namespace Wallnut {
	class WALLNUT_API Application
	{
	private:

		HWND m_hWnd = NULL;

		int clientWidth = 800;
		int clientHeight = 600;

		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

		Graphics* graphics = NULL;

#pragma region WndProc

		LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma endregion

	protected:

		HWND getHwnd() const { return m_hWnd; }
		Graphics* getGraphics() const { return graphics; }

		const std::wstring WindowTitle = L"Game";
		const std::wstring WindowClassName = L"Wallnut Game Window";


	public:

		Application(const Application& copy) = delete;
		Application() {  };
		Application(int windowWidth, int windowHeight);
		~Application();

		void Render(Graphics& graphics) { }

		int Run();
		void Quit();

		virtual void Init() = 0;
	};

	Application* CreateApplication();
}

