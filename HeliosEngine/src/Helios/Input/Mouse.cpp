/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#include "pch.h"
#include "Mouse.h"
#include "Helios/Math/Vector.h"
#include <GLFW/glfw3.h>
#include "Helios/Core/Application.h"

using namespace Helios;

void Mouse::SetCursorState(MouseState state)
{
	switch (state)
	{
	case Mouse::Locked:
#ifdef HELIOS_PLATFORM_WINDOWS
		//SetCapture(DepricatedApplication::hWnd);
		RECT cRect;
		//GetWindowRect(DepricatedApplication::hWnd, &cRect);
		ClipCursor(&cRect);
#else
        glfwSetInputMode((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
        break;
    case Mouse::Show:
        glfwSetInputMode((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case Mouse::Hidden:
        glfwSetInputMode((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;

	default:
		break;
	}
}

Point Mouse::GetPosition()
{
    double xpos, ypos;
    glfwGetCursorPos((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), &xpos, &ypos);
    return Point(static_cast<int>(xpos), static_cast<int>(ypos));
}