#pragma once

#include "pch.h"
#include "Helios/Core/Window.h"
#include "Helios/Core/Asserts.h"

#ifdef HELIOS_PLATFORM_WINDOWS

namespace Helios
{
	class Win32Window : public Window
	{
	public:
		void Show(bool) override;
		void Hide() override;
		bool IsFocused() override;
		bool Create(Specifications) override;

		HWND GetHWND() const;

		Win32Window* GetWin32WindowPtr() { return this; }
		Win32Window& GetWin32WindowRef() { return *this; }

		virtual const wchar_t* ClassName() const = 0;
	protected:
		virtual LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		DWORD m_dwStyle = NULL;
		DWORD m_dwExStyle = NULL;
		UINT m_wcStyle = NULL;
		HWND m_hWnd = NULL;
	};
}

#endif