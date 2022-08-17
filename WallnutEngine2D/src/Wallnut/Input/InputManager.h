/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

namespace Wallnut 
{
	class Point;
	class InputManager {
	public:
		static bool IsKeyPressed(int key);
		static bool IsKeyPressedDown(int key);
		static bool IsKeyPressedUp(int key);
		static bool IsKeyToggled(int key);
		static Point GetMousePosition();
	};
}