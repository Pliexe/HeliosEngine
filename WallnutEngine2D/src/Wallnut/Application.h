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

		static HWND hWnd;

		HWND m_hWnd = NULL;
		RAWINPUTDEVICE rid = { };

		int clientWidth = 800;
		int clientHeight = 600;

		float baseCanvas = 3600;

		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

		Graphics* graphics = NULL;

		static Application* instance;

#pragma region WndProc

		LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma endregion

		void HandleGameLoop();

	protected:

		HWND getHwnd() const { return m_hWnd; }
		Graphics* getGraphics() const { return graphics; }

		const std::wstring WindowTitle = L"Game";
		const std::wstring WindowClassName = L"Wallnut Game Window";

		bool fpsCounter = false;

	public:

		Application(const Application& copy) = delete;
		Application() { };
		Application(int windowWidth, int windowHeight);
		~Application();

		void Render(Graphics& graphics) { }

		int Run();
		static void ShoWNessageBox(const wchar_t* title, const wchar_t* text, UINT type = MB_OK, bool pauseGame = true);
		static void Quit();

		virtual void Init() = 0;

		friend class Mouse;
		friend class InputManager;
		friend class Camera;
	};

	Application* CreateApplication();
}

