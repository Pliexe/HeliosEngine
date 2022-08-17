/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Application.h"
#include "Time.h"
#include "pch.h"
#include <Windows.h>

//Wallnut::Application::Application(bool fullscreen)
//{
//	GetMonitorInfo()
//}

HWND Wallnut::Application::hWnd = NULL;
Wallnut::Application* Wallnut::Application::instance = NULL;

LRESULT Wallnut::Application::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_LBUTTONUP:
		//if (currentGameSection) currentGameSection->OnClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_SIZE:
	case WM_SIZING:
	{
		if (graphics) {
			RECT client_rect;
			GetClientRect(m_hWnd, &client_rect);
			clientWidth = client_rect.right;
			clientHeight = client_rect.bottom;
			graphics->renderTarget->Resize(D2D1::SizeU(client_rect.right, client_rect.bottom));

			auto renderTarget = graphics->renderTarget;
			float tmp = (baseCanvas - (baseCanvas - (clientWidth + clientHeight))) / baseCanvas;
			//std::cout << "Scale: " << tmp << ", DIFF" << (baseCanvas - (baseCanvas - (clientWidth + clientHeight))) << std::endl;
			renderTarget->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::SizeF(tmp, tmp)));

			HandleGameLoop();
		}
		return 0;
	}
	default:
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		break;
	}
}

LRESULT Wallnut::Application::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Application* pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (Application*)pCreate->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;
	}
	else pThis = (Application*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (pThis)
		return pThis->WindowProc(uMsg, wParam, lParam);
	else return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Wallnut::Application::Application(int windowWidth, int windowHeight)
{
	clientWidth = 800;
	clientHeight = 600;
}

Wallnut::Application::~Application()
{
	if (graphics) delete graphics;
}

int Wallnut::Application::Run()
{

	Application::instance = this;
	
#pragma region Init Counter

	Time::Init();

#pragma endregion

#pragma region Check if the Window Class already exists and if not create it

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	if (!GetClassInfoEx(GetModuleHandle(NULL), WindowClassName.c_str(), &wc)) {
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = Application::StaticWindowProc;
		wc.lpszClassName = WindowClassName.c_str();
		wc.style = CS_HREDRAW | CS_VREDRAW;

		RegisterClassEx(&wc);
	}

#pragma endregion

#pragma region Create Window

	// Calculate the size to set for the actual window since client area always shrinks. This is done by AdjustWindowRect which is calculated by the os
	RECT rect = { 0, 0, clientWidth, clientHeight };
	AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

	m_hWnd = CreateWindowEx(
		dwExStyle, WindowClassName.c_str(), WindowTitle.c_str(), dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, GetModuleHandle(NULL), this
	);
	Application::hWnd = m_hWnd;

	if (!m_hWnd) {
		MessageBox(m_hWnd, L"Failed to Construct Window!", L"Error!", MB_ICONERROR);
		return -2;
	}

#pragma endregion

#pragma region Init Graphics

	graphics = new Graphics(m_hWnd);
	if (!graphics->Init()) {
		std::system_category().message(GetLastError());
		return -3;
	}

#pragma endregion

	Init();

	GameObject::prepare(); // increase reserved space

	ShowWindow(m_hWnd, SW_SHOW);

#pragma region Game Loop

	MSG message;
	message.message = WM_NULL;

	while (message.message != WM_QUIT) {
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else {
			HandleGameLoop();
		}
	}

#pragma endregion

	return 0;
}

void Wallnut::Application::ShoWNessageBox(const wchar_t* title, const wchar_t* text, UINT type, bool pauseGame)
{
	MessageBox(pauseGame ? instance->m_hWnd : NULL, text, title, type);
}

void Wallnut::Application::HandleGameLoop() {

	if (SceneManager::currentScene) {

#pragma region Clean Up

		GameObject::CheckQueue(*graphics);
		SceneManager::CheckQueue();

#pragma endregion

#pragma region Update Loop

		Time::frameUpdate();

		SceneManager::Update();

#pragma endregion

#pragma region Render Loop

		graphics->renderTarget->BeginDraw();

		SceneManager::Render(*graphics);

		Render(*graphics);

		graphics->renderTarget->EndDraw();

#pragma endregion

	}

}


void Wallnut::Application::Quit()
{
	PostMessage(Application::instance->m_hWnd, WM_CLOSE, 0, 0);
}
