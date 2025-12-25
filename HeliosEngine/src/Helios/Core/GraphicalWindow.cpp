#include "GraphicalWindow.h"
#include <GLFW/glfw3.h>

#if defined(HELIOS_PLATFORM_WINDOWS)
#include "Platform/Windows/Win32GraphicalWindow.h"
#elif defined(HELIOS_PLATFORM_LINUX)
#include "Platform/Linux/LinuxGraphicalWindow.h"
#else
#error "Not implemented!"
#endif

namespace Helios
{
	// Empty template for the future implementation of the GraphicalWindow class
	Ref<GraphicalWindow> GraphicalWindow::Create()
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return CreateRef<Win32GraphicalWindow>(device);
#elif HELIOS_PLATFORM_LINUX
        return CreateRef<Internal::LinuxGraphicalWindow>();
#else
		HELIOS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

	Unique<GraphicalWindow> GraphicalWindow::CreateScoped()
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return CreateScope<Win32GraphicalWindow>();
#elif HELIOS_PLATFORM_LINUX
		return CreateUnique<Internal::LinuxGraphicalWindow>();
#else
		HELIOS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

	void GraphicalWindow::Message(const std::string &title, const std::string &message)
	{
		#if defined(HELIOS_PLATFORM_WINDOWS)
			::MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ICONERROR);
		#elif defined(HELIOS_PLATFORM_LINUX)
			// GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message.c_str());
		    // gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
		    // gtk_dialog_run(GTK_DIALOG(dialog));
		    // gtk_widget_destroy(dialog);
		#endif
	}
	void GraphicalWindow::PollEvents()
	{
#if defined(HELIOS_PLATFORM_WINDOWS) || defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
		glfwPollEvents();
#else
#endif
	}
}
