/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "pch.h"
#include "Mouse.h"
#include "Helios/Core/DepricatedApplication.h"
#include "Helios/Translation/Vector.h"

using namespace Helios;

void Mouse::SetCursorState(MouseState state)
{
	switch (state)
	{
	case Mouse::Locked:
		//SetCapture(DepricatedApplication::hWnd);
		RECT cRect;
		GetWindowRect(DepricatedApplication::hWnd, &cRect);
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
		if (ScreenToClient(DepricatedApplication::instance->m_hWnd, &p)) {
			return Point(p.x, p.y);
		}
	}
	return Point();
}
