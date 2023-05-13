/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "pch.h"

#include "InputManager.h"
#include "Helios/Translation/Vector.h"
#include "Helios/Core/DepricatedApplication.h"

bool Helios::InputManager::IsKeyPressed(int key)
{
    return GetAsyncKeyState(key);
}

bool Helios::InputManager::IsKeyDown(int key)
{
    return GetAsyncKeyState(key) < 0;
}

bool Helios::InputManager::IsKeyUp(int key)
{
    return GetAsyncKeyState(key) > 0;
}

bool Helios::InputManager::IsKeyToggled(int key)
{
    return GetAsyncKeyState(key) & 0x0001;
}

Helios::Point Helios::InputManager::GetMousePosition()
{
    POINT p;
    if (GetCursorPos(&p)) {
        if (ScreenToClient(Helios::DepricatedApplication::instance->m_hWnd, &p)) {
            return Point(p.x, p.y);
        }
    }
    return Point();
}

void Helios::InputManager::OnUpdate()
{
    s_MouseWheelDelta = 0.0f;
}
