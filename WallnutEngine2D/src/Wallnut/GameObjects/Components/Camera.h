/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/GameObjects/ObjectComponent.h"
#include "Wallnut/Resources/Color.h"

namespace Wallnut {
	class ObjectComponent;
	class WALLNUT_API Camera : public ObjectComponent {
	private:

		Color backgroundColor = Color::Black;

	public:

		void SetBackgroundColor(int r, int g, int b, float a = 1.0f) { backgroundColor = Color(r / 255.0f, g / 255.0f, b / 255.0f, a); }
		Color GetBackgroundColor() const { return backgroundColor; }
		void SetBackgroundColor(float r, float  g, float b, float a = 1.0f) { backgroundColor = Color(r, g, b, a); }

		Vector2D ScreenToWorldPoint(Point mousePosition);

		friend class SceneManager;
		friend class Transform;

		extern friend class GameEngine;
	};
}