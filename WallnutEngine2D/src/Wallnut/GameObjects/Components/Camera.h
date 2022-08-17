/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/GameObjects/ObjectComponent.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class ObjectComponent;
	class WALLNUT_API Camera : public ObjectComponent {
	private:

		D2D1_COLOR_F backgroundColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);

	public:

		void SetBackgroundColor(int r, int g, int b, float a = 1.0f) { backgroundColor = D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a); }
		void SetBackgroundColor(float r, float  g, float b, float a = 1.0f) { backgroundColor = D2D1::ColorF(r, g, b, a); }

		Vector2D ScreenToWorldPoint(Point mousePosition);

		friend class SceneManager;
		friend class Transform;
	};
}