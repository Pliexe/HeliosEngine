/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Wallnut/Core.h"

namespace Wallnut {
	class Point;
	class WALLNUT_API Mouse {
	private:
	public:

		enum MouseState {
			Locked,
			Show
		};

		static void SetCursorState(MouseState state);

		static Point GetPosition();
	};
}