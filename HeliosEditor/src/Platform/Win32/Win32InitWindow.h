#pragma once
#include "pch.h"
#include "InitWindow.h"

#ifdef HELIOS_PLATFORM_WINDOWS

namespace Helios
{
	class Win32InitWindow final : public InitWindow
	{
	public:
		Win32InitWindow();
		virtual void Create(uint32_t width, uint32_t height) override;
		virtual void SetTextA(std::string text) override;
		virtual void SetTextB(std::string text) override;
		virtual void SetProgressA(float progress) override;
		virtual void SetProgressB(float progress) override;
		virtual void ShowMessage(const char* title, const char* text) override;
		virtual void Quit() override;
	private:
		// winproc
		static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void InternalInit(uint32_t width, uint32_t height);
	public:

	private:
		HWND m_hwnd = nullptr;

		float m_progressA = 0.0f;
		float m_progressB = 0.0f;
		std::wstring m_textA;
		std::wstring m_textB;

		bool m_quit = false;
	};

	
}

#endif