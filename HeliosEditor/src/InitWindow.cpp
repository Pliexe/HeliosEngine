#include "InitWindow.h"

#ifdef HELIOS_PLATFORM_WINDOWS
#include "Platform/Win32/Win32InitWindow.h"
#endif

namespace Helios
{
	InitWindow* InitWindow::Create()
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return (InitWindow*) new Win32InitWindow();
#else
		assert(false);
		return nullptr;
#endif
	}
}
