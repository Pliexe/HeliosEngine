#pragma once
#include "pch.h"
#include "InitWindow.h"

namespace Helios
{
	class Win32InitWindow : public InitWindow
	{
	public:
		void Create(uint32_t width, uint32_t height) override;
		void SetTextA(const char* text) override;
		void SetTextB(const char* text) override;
		void SetProgressA(float progress) override;
		void SetProgressB(float progress) override;
		void MessageBox(const char* title, const char* text) override;
		void Quit() override;
	private:
		// winproc
		static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_hwnd = nullptr;

		float m_progressA = 0.0f;
		float m_progressB = 0.0f;
		const char* m_textA = "";
		const char* m_textB = "";
	};

	
}
