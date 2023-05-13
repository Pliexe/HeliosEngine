/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Base.h"
#include "GraphicalWindow.h"
#include "Helios/Graphics/DepricatedGraphics.h"
#include "Helios/Scene/SceneRegistry.h"

namespace Helios {

	class HELIOS_API WindowCordinates {
	private:
		static float width;
		static float height;
		static float scale;
	public:
		WindowCordinates() = delete;
		WindowCordinates(const WindowCordinates&) = delete;
		
		static void SetSize(int w, int h) { 
			static float baseCanvas = 1920 + 1680;
			width = w; height = h;
			scale = (baseCanvas - (baseCanvas - (w + h))) / baseCanvas;
		}

		static inline const float getScale() { return scale; }

		static inline Vector2 getSize() { return { width, height }; }

		static inline const float getWidth() { return width; }
		static inline const float getHeight() { return height; }

		friend class DepricatedApplication;
		extern friend class EngineScene;
	};

	class HELIOS_API DepricatedApplication
	{
	private:

		static HWND hWnd;

		HWND m_hWnd = NULL; // Window Handle
		RAWINPUTDEVICE rid = { };

		int clientWidth = 800;
		int clientHeight = 600;

		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

		DepricatedGraphics* graphics = NULL;

		static DepricatedApplication* instance;

#pragma region WndProc

		virtual LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma endregion

		void GameLoop();

		void Shutdown();

	protected:

		DepricatedGraphics* getGraphics() const { return graphics; }

		static const wchar_t* WindowTitle() { return L"Game"; };
		static const wchar_t* WindowClassName() { return L"Helios Game Window"; };

		bool fpsCounter = false;

		Scope<GraphicalWindow> m_window;

	public:
		static const HWND GetHwnd() { return instance->m_hWnd; }

		DepricatedApplication(const DepricatedApplication& copy) = delete;
		DepricatedApplication() { };
		DepricatedApplication(int windowWidth, int windowHeight);
		~DepricatedApplication();

		void Render(DepricatedGraphics& graphics) { }

		virtual void OnRender();
		virtual void OnUpdate();
		void CheckEngineQueue();

		int Run();
		static void ShowMessageBox(const wchar_t* title, const wchar_t* text, UINT type = MB_OK, bool pauseGame = true);

		static void ShowMessage(const wchar_t* title, const wchar_t* text, UINT type = MB_OK, bool pauseGame = true);
		static void ShowMessage(std::wstring title, std::wstring text, UINT type = MB_OK, bool pauseGame = true);
		static void ShowMessage(const char* title, const char* text, UINT type = MB_OK, bool pauseGame = true);
		static void ShowMessage(std::string title, std::string text, UINT type = MB_OK, bool pauseGame = true);

		static void Quit();

		virtual void Init() = 0;
#ifdef HELIOS_EDITOR
		virtual void OnGizmosRender() = 0;
#endif

		friend class Mouse;
		friend class InputManager;
		friend class Camera;
		friend class Bitmap;
		friend class Scene;

		extern friend class DepricatedGameEngine;
	};

	/*DepricatedApplication* CreateApplication();*/
}

