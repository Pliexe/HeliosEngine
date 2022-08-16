/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Graphics/Graphics.h"

namespace Wallnut {
	class Brush
	{
	public:
		virtual ID2D1Brush* createBrush(Graphics& graphics) = 0;
	};
}

