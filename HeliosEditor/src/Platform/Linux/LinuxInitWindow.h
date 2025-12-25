#pragma once
#include "pch.h"
#include "InitWindow.h"

#ifdef HELIOS_PLATFORM_LINUX
#include <X11/Xlib.h>
#include <cairo/cairo.h>

namespace Helios
{
	class LinuxInitWindow final : public InitWindow
	{
	public:
		LinuxInitWindow();
		virtual void Create(uint32_t width, uint32_t height) override;
		virtual void SetTextA(std::string text) override;
		virtual void SetTextB(std::string text) override;
		virtual void SetProgressA(float progress) override;
		virtual void SetProgressB(float progress) override;
		virtual void ShowMessage(const char* title, const char* text) override;
		virtual void Quit() override;

	private:
		void InternalInit(uint32_t width, uint32_t height);
		void Redraw();
		static void* ThreadEntry(void* data);

	private:
		::Display* m_display = nullptr;
		::Window m_window = 0;

		cairo_surface_t* m_surface = nullptr;
		cairo_t* m_cr = nullptr;

		std::wstring m_textA;
		std::wstring m_textB;
		float m_progressA = 0.0f;
		float m_progressB = 0.0f;

		uint32_t m_width = 400;
		uint32_t m_height = 280;
		bool m_quit = false;
		bool m_windowCreated = false;
		pthread_t m_thread;
		std::mutex m_mutex;
	};
}

#endif
