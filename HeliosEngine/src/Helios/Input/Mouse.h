/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Helios/Core/Base.h"

namespace Helios {
	class Point;
	class HELIOS_API Mouse {
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