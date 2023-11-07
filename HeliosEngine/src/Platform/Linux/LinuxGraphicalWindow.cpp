#include "LinuxGraphicalWindow.h"

namespace Helios
{
	void LinuxGraphicalWindow::SetVSync(bool enabled)
	{
		m_Context->SetVSync(enabled);
	}

	void LinuxGraphicalWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
	}
}
