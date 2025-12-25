#include "InitWindow.h"

#if defined(HELIOS_PLATFORM_WINDOWS)
#include "Platform/Win32/Win32InitWindow.h"
#elif defined(HELIOS_PLATFORM_LINUX)
#include "Platform/Linux/LinuxInitWindow.h"
#endif

namespace Helios
{
	InitWindow* InitWindow::Create()
	{
#if defined(HELIOS_PLATFORM_WINDOWS)
		return (InitWindow*) new Win32InitWindow();

#elif defined(HELIOS_PLATFORM_LINUX)
		return (InitWindow*) new LinuxInitWindow();
#else
		assert(false);
		return nullptr;
#endif
	}
}
