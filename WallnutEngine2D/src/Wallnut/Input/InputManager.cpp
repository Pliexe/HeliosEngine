/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "InputManager.h"
#include "Wallnut/Translation/Vector2D.h"
#include "Wallnut/Application.h"
#include "pch.h"

bool Wallnut::InputManager::IsKeyPressed(int key)
{
    return GetAsyncKeyState(key);
}

bool Wallnut::InputManager::IsKeyPressedDown(int key)
{
    return GetAsyncKeyState(key) < 0;
}

bool Wallnut::InputManager::IsKeyPressedUp(int key)
{
    return GetAsyncKeyState(key) > 0;
}

bool Wallnut::InputManager::IsKeyToggled(int key)
{
    return GetAsyncKeyState(key) & 0x0001;
}

Wallnut::Point Wallnut::InputManager::GetMousePosition()
{
    POINT p;
    if (GetCursorPos(&p)) {
        if (ScreenToClient(Wallnut::Application::instance->m_hWnd, &p)) {
            return Point(p.x, p.y);
        }
    }
    return Point();
}
