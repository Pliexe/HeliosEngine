/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#include "pch.h"

#include "InputManager.h"
#include "Helios/Math/Vector.h"
#include "Helios/Core/Application.h"

#if defined(HELIOS_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
#include <GLFW/glfw3.h>
#endif

bool Helios::InputManager::IsKeyPressed(int key)
{
#ifdef HELIOS_PLATFORM_WINDOWS
    return GetAsyncKeyState(key);
#elif defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
    return glfwGetKey((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), key) == GLFW_PRESS;
#else
#error "Platform not supported!"
#endif
}

bool Helios::InputManager::IsKeyDown(int key)
{
#ifdef HELIOS_PLATFORM_WINDOWS
    return GetAsyncKeyState(key) < 0;
#elif defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
    return glfwGetKey((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), key) == GLFW_PRESS;
#else
#error "Platform not supported!"
#endif
}

bool Helios::InputManager::IsKeyUp(int key)
{
#ifdef HELIOS_PLATFORM_WINDOWS
    return GetAsyncKeyState(key) > 0;
#elif defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
    return glfwGetKey((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), key) == GLFW_RELEASE;
#else
#error "Platform not supported!"
#endif
}

bool Helios::InputManager::IsKeyToggled(int key)
{
#ifdef HELIOS_PLATFORM_WINDOWS
    return GetAsyncKeyState(key) & 0x0001;
#elif defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
    return false;
#else
#error "Platform not supported!"
#endif
}

Helios::Point Helios::InputManager::GetMousePosition()
{
    // POINT p;
    // /*if (GetCursorPos(&p)) {
    //     if (ScreenToClient(Helios::DepricatedApplication::instance->m_hWnd, &p)) {
    //         return Point(p.x, p.y);
    //     }
    // }*/
    // return Point();

#if defined(HELIOS_PLATFORM_WINDOWS) || defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
    double xpos, ypos;
    glfwGetCursorPos((GLFWwindow*)Application::GetInstance().GetWindow()->GetNativeWindow(), &xpos, &ypos);
    return Point((int)xpos, (int)ypos);
#elif 
#error "Platform not supported!"
#endif
}

void Helios::InputManager::OnUpdate()
{
    s_MouseWheelDelta = 0.0f;
}
