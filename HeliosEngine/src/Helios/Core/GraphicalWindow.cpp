#include "GraphicalWindow.h"

#include "Platform/Windows/Win32GraphicalWindow.h"

namespace Helios
{
	// Empty template for the future implementation of the GraphicalWindow class
	Ref<GraphicalWindow> GraphicalWindow::Create()
	{
#ifdef HELIOS_PLATFORM_WINDOWS
		return CreateRef<Win32GraphicalWindow>();
#else
		HELIOS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
