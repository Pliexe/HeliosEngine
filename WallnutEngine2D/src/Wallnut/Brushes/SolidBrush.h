/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "Brush.h"

namespace Wallnut {
	class SolidBrush : public Brush {
	private:
		float m_r;
		float m_g;
		float m_b;
		float m_a;

	public:
		SolidBrush() = delete;
		SolidBrush(int r, int g, int b, float a = 1) : m_r(r / 255.0f), m_g(g / 255.0f), m_b(b / 255.0f), m_a(a) { }
		SolidBrush(float r, float g, float b, float a = 1) : m_r(r), m_g(g), m_b(b), m_a(a) { }

		ID2D1Brush* createBrush(Graphics& g) override {
			ID2D1SolidColorBrush* tmp = NULL;
			g.getRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(m_r, m_g, m_b, m_a), &tmp);
			return tmp;
		}
	};
}