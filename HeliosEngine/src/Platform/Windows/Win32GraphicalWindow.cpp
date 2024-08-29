#include "Win32GraphicalWindow.h"

namespace Helios
{
	void Win32GraphicalWindow::SetVSync(bool enabled)
	{
		m_Context->SetVSync(enabled);
	}

	bool Win32GraphicalWindow::VSyncEnabled() const
	{
		return m_Context->GetVSyncInterval();
	}

	void Win32GraphicalWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
	}
}
