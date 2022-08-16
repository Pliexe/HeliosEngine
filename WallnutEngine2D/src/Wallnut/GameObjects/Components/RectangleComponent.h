/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/GameObjects/ObjectComponent.h"
#include "Wallnut/Brushes/Brush.h"
#include "Wallnut/Brushes/SolidBrush.h"

namespace Wallnut {
	class RectangleComponent : public ObjectComponent {
	private:

		float stroke = 0.0f;

		ID2D1StrokeStyle* strokeStyle = NULL;
		ID2D1Brush* m_brush = NULL;
		std::unique_ptr<Brush> m_brushTemplate;

		void Init(Graphics& g) override {
			m_brush = m_brushTemplate.get()->createBrush(g);
		}

		void Render(Graphics& g) override {
			if (m_brush != NULL)
			{
				if(stroke <= 0.0f)
					g.getRenderTarget()->FillRectangle(gameObject->getTransform().getRectF(), m_brush);
				else 
					g.getRenderTarget()->DrawRectangle(gameObject->getTransform().getRectF(), m_brush, stroke);
			}
		}

	public:

		RectangleComponent() = delete;
		RectangleComponent(SolidBrush brush, float strokeWidth = 0.0f) {
			m_brushTemplate = std::make_unique<SolidBrush>(brush);
			this->stroke = strokeWidth;
		}

		friend class Application;
		friend class SceneManager;
	};
}