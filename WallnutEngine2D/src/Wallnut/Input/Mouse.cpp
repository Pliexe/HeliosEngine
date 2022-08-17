/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "pch.h"
#include "Mouse.h"
#include "Wallnut/Application.h"
#include "Wallnut/Translation/Vector2D.h"

using namespace Wallnut;

void Mouse::SetCursorState(MouseState state)
{
	switch (state)
	{
	case Mouse::Locked:
		//SetCapture(Application::hWnd);
		RECT cRect;
		GetWindowRect(Application::hWnd, &cRect);
		ClipCursor(&cRect);
		break;
	case Mouse::Show:
		break;
	default:
		break;
	}
}

Point Mouse::GetPosition()
{
	POINT p;
	if (GetCursorPos(&p)) {
		if (ScreenToClient(Application::instance->m_hWnd, &p)) {
			return Point(p.x, p.y);
		}
	}
	return Point();
}
