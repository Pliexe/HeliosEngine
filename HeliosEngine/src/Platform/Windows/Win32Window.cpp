#include "Win32Window.h"

namespace Helios
{
	void Win32Window::Show(bool fullscreen)
	{
		ShowWindow(m_hWnd, fullscreen ? SW_SHOWMAXIMIZED : SW_SHOW);
	}

	void Win32Window::Hide()
	{
		HELIOS_ASSERT(m_hWnd);
		ShowWindow(m_hWnd, SW_HIDE);
	}

	bool Win32Window::IsFocused()
	{
		HELIOS_ASSERT(m_hWnd);
		return GetForegroundWindow() == m_hWnd;
	}

	LRESULT Win32Window::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Win32Window* pThis = nullptr;

		if (uMsg == WM_NCCREATE)
		{
			pThis = static_cast<Win32Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		} else pThis = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (pThis) return pThis->WindowProc(uMsg, wParam, lParam);
		else return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool Win32Window::Create(Specifications specs)
	{
		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		if(!GetClassInfoEx(hInstance, ClassName(), &wc))
		{
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = m_wcStyle;
			wc.lpfnWndProc = StaticWindowProc;
			wc.hInstance = hInstance;
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = ClassName();
			RegisterClassEx(&wc);
		}

		RECT wr = { 0, 0, (long)specs.width, (long)specs.height };
		AdjustWindowRectEx(&wr, m_dwStyle, FALSE, m_dwExStyle);

		m_hWnd = CreateWindowEx(
			m_dwExStyle,
			ClassName(),
			std::wstring(specs.title.begin(), specs.title.end()).c_str(),
			m_dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			wr.right - wr.left, wr.bottom - wr.top,
			nullptr,
			nullptr,
			hInstance,
			this
		);

		return m_hWnd != nullptr;
	}

	HWND Win32Window::GetHWND() const
	{
		HELIOS_ASSERT(m_hWnd);
		return m_hWnd;
	}
}
