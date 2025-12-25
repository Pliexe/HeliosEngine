#include "LinuxInitWindow.h"

#ifdef HELIOS_PLATFORM_LINUX

#include <cairo/cairo-xlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

namespace Helios
{
	LinuxInitWindow::LinuxInitWindow()
	{
		Create(400, 280);
	}

	void LinuxInitWindow::Create(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;
		pthread_create(&m_thread, nullptr, ThreadEntry, this);
	}

	void* LinuxInitWindow::ThreadEntry(void* data)
	{
		static_cast<LinuxInitWindow*>(data)->InternalInit(
			static_cast<LinuxInitWindow*>(data)->m_width,
			static_cast<LinuxInitWindow*>(data)->m_height);
		return nullptr;
	}

	void LinuxInitWindow::SetTextA(std::string text)
	{
		{
            std::lock_guard<std::mutex> lock(m_mutex);
		    m_textA = std::wstring(text.begin(), text.end());
        }
		Redraw();
	}

	void LinuxInitWindow::SetTextB(std::string text)
	{
		{
            std::lock_guard<std::mutex> lock(m_mutex);
		    m_textB = std::wstring(text.begin(), text.end());
        }
		Redraw();
	}

	void LinuxInitWindow::SetProgressA(float progress)
	{
		{
            std::lock_guard<std::mutex> lock(m_mutex);
		    m_progressA = progress;
        }
		Redraw();
	}

	void LinuxInitWindow::SetProgressB(float progress)
	{
		{
            std::lock_guard<std::mutex> lock(m_mutex);
		    m_progressB = progress;
        }
		Redraw();
	}

	void LinuxInitWindow::ShowMessage(const char* title, const char* text)
	{
		std::cerr << title << ": " << text << std::endl;
	}

	void LinuxInitWindow::Quit()
	{
		m_quit = true;
		// XDestroyWindow(m_display, m_window);
	}

	void LinuxInitWindow::InternalInit(uint32_t width, uint32_t height)
	{
		// XInitThreads();
		// m_display = XOpenDisplay(nullptr);
		// if (!m_display) return;

		// int screen = DefaultScreen(m_display);
		// Window root = RootWindow(m_display, screen);

		// m_window = XCreateSimpleWindow(m_display, root, 100, 100, width, height, 1,
		// 							   BlackPixel(m_display, screen), WhitePixel(m_display, screen));

		// XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
		// XMapWindow(m_display, m_window);

		// m_surface = cairo_xlib_surface_create(m_display, m_window,
		// 									  DefaultVisual(m_display, screen),
		// 									  width, height);
		// m_cr = cairo_create(m_surface);
		// m_windowCreated = true;

		// XEvent event;
		// while (!m_quit)
		// {
		// 	while (XPending(m_display) > 0)
		// 	{
		// 		XNextEvent(m_display, &event);
		// 		if (event.type == Expose)
		// 			Redraw();
		// 	}
		// 	usleep(10000); // idle
		// }
		
		// cairo_surface_flush(m_surface);  
		// cairo_destroy(m_cr);
		// cairo_surface_destroy(m_surface);
		// // XSync(m_display, False);
		// XCloseDisplay(m_display);
	}

	void LinuxInitWindow::Redraw()
	{
		if (!m_windowCreated) return;

		std::lock_guard<std::mutex> lock(m_mutex);

		cairo_set_source_rgb(m_cr, 0.145, 0.141, 0.18);
		cairo_paint(m_cr);

		cairo_set_source_rgb(m_cr, 1.0, 1.0, 1.0);
		cairo_select_font_face(m_cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(m_cr, 14);
		cairo_move_to(m_cr, 20, 30);
		cairo_show_text(m_cr, std::string(m_textA.begin(), m_textA.end()).c_str());

		cairo_rectangle(m_cr, 50, 60, 300, 4);
		cairo_set_source_rgb(m_cr, 0.4, 0.4, 0.4);
		cairo_fill(m_cr);

		cairo_rectangle(m_cr, 50, 60, 300 * m_progressA, 4);
		cairo_set_source_rgb(m_cr, 0.7, 0.7, 1.0);
		cairo_fill(m_cr);

		if (!m_textB.empty())
		{
			cairo_set_font_size(m_cr, 12);
			cairo_move_to(m_cr, 20, 90);
			cairo_show_text(m_cr, std::string(m_textB.begin(), m_textB.end()).c_str());

			cairo_rectangle(m_cr, 50, 110, 300, 4);
			cairo_set_source_rgb(m_cr, 0.4, 0.4, 0.4);
			cairo_fill(m_cr);

			cairo_rectangle(m_cr, 50, 110, 300 * m_progressB, 4);
			cairo_set_source_rgb(m_cr, 0.7, 0.7, 1.0);
			cairo_fill(m_cr);
		}

		cairo_surface_flush(m_surface);
		XFlush(m_display);
	}
}

#endif
