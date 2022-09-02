/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Core.h"

namespace Wallnut {

	class WALLNUT_API StrokeStyle {
	private:
		ID2D1StrokeStyle* style = NULL;
	public:

		

		operator ID2D1StrokeStyle* () { return style; }
	};

	class WALLNUT_API Brush
	{
	protected:
		std::shared_ptr<ID2D1Brush> brush;
		static void releaseBrush(ID2D1Brush* b) { b->Release(); }
	public:

		enum class BrushType {
			Solid,
			LinearGradient,
			RadialGradient,
			Bitmap
		};

		inline bool isReady() { return brush.get() != nullptr; }
		operator ID2D1Brush* () { return brush.get(); }
		ID2D1Brush *operator -> () {
			return brush.get();
		}

		virtual inline bool isPainted() const = 0;
		virtual inline BrushType getType() const = 0;
	};
}

