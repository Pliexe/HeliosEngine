#include "GraphicalWindow.h"

#include "Platform/Windows/Win32GraphicalWindow.h"
//#include "Platform/Windows/LinuxGraphicalWindow.h"

namespace Helios
{
	// Empty template for the future implementation of the GraphicalWindow class
	Ref<GraphicalWindow> GraphicalWindow::Create()
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return CreateRef<Win32GraphicalWindow>();
#elif HELIOS_PLATFORM_LINUX
        return CreateRef<LinuxGraphicalWIndow>()
#else
		HELIOS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

	void GraphicalWindow::MessageBox(const std::string &title, const std::string &message)
	{
		#if defined(HELIOS_PLATFORM_WINDOWS)
			MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ICONERROR);
		#elif defined(HELIOS_PLATFORM_LINUX)
			GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message.c_str());
		    gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
		    gtk_dialog_run(GTK_DIALOG(dialog));
		    gtk_widget_destroy(dialog);
		#endif
	}
}
