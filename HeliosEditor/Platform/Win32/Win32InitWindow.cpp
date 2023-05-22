#include "Win32InitWindow.h"

namespace Helios
{
	inline void Win32InitWindow::Create(uint32_t width, uint32_t height)
	{
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		if (!GetClassInfoEx(GetModuleHandle(nullptr), L"HeliosInitWindow", &wc))
		{
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = StaticWndProc;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = L"HeliosInitWindow";
			RegisterClassEx(&wc);
		}

		RECT wr = { 0, 0, width, height };

		m_hwnd = CreateWindowEx(
			0,
			L"HeliosInitWindow",
			L"Helios Engine",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);

		ShowWindow(m_hwnd, SW_SHOW);
	}

	inline void Win32InitWindow::SetTextA(const char* text)
	{
		m_textA = text;
	}

	inline void Win32InitWindow::SetTextB(const char* text)
	{
		m_textB = text;
	}

	inline void Win32InitWindow::SetProgressA(float progress)
	{
		m_progressA = progress;
	}

	inline void Win32InitWindow::SetProgressB(float progress)
	{
		m_progressB = progress;
	}

	inline void Win32InitWindow::MessageBox(const char* title, const char* text)
	{
		MessageBoxA(m_hwnd, text, title, MB_OK);
	}

	inline void Win32InitWindow::Quit()
	{
	}

	LRESULT Win32InitWindow::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{


		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
