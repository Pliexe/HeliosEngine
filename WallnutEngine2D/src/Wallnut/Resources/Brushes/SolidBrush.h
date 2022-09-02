/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Brush.h"
#include "../Color.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class WALLNUT_API SolidBrush : public Brush {
	private:

	public:
		SolidBrush() = delete;
		SolidBrush(Color color);

		SolidBrush(int r, int g, int b, float a = 1.0f) : SolidBrush(r / 255.0f, g / 255.0f, b / 255.0f, a) { }
		SolidBrush(float r, float g, float b, float a = 1.0f);

		// Inherited via Brush
		virtual bool isPainted() const override;
		virtual BrushType getType() const override;

		operator ID2D1SolidColorBrush* () { return (ID2D1SolidColorBrush*)brush.get(); }
		ID2D1SolidColorBrush* operator -> () {
			return (ID2D1SolidColorBrush*)brush.get();
		}
	};
}