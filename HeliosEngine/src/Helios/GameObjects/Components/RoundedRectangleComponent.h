/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/GameObjects/ObjectComponent.h"
#include "Helios/Resources/Brushes/Brush.h"
#include "Helios/Resources/Brushes/BitmapBrush.h"

namespace Helios {
	class RoundedRectangleComponent : public ObjectComponent {
	private:
		
		float radiusY;
		float radiusX;
		float stroke = 0.0f;
		ID2D1StrokeStyle* strokeStyle = NULL;
		std::unique_ptr<Brush> m_brush;

		void Render(Graphics& g) override {
			if (m_brush->isReady())
			{
				auto pos = transform->getScreenPosition();
				if (m_brush->isPainted())
				{
					auto brush = dynamic_cast<BitmapBrush*>(m_brush.get());
					if (brush) {
						auto size = brush->getSize();
						(*brush)->SetTransform(
							D2D1::Matrix3x2F::Scale(D2D1::SizeF((pos.w - pos.x) / size.width, (pos.h - pos.y) / size.height)) *
							D2D1::Matrix3x2F::Translation(D2D1::SizeF(pos.x, pos.y))
						);
					}
				}

				if (stroke <= 0.0f)
					g.getRenderTarget()->FillRoundedRectangle(D2D1::RoundedRect(pos, radiusX, radiusY), *m_brush);
				else
					g.getRenderTarget()->DrawRoundedRectangle(D2D1::RoundedRect(pos, radiusX, radiusY), *m_brush, stroke);
			}
		}

	public:

		RoundedRectangleComponent() = delete;
		template <class T = Brush>
		RoundedRectangleComponent(T brush, float radiusX = 5f, float radiusY = 5f, float strokeWidth = 0.0f) {
			static_assert(std::is_base_of<Brush, T>::value, "Type T must be derived from Brush!");
			m_brush = std::make_unique<T>(brush);
			this->stroke = strokeWidth;

			this->radiusX = radiusX;
			this->radiusY = radiusY;
		}

		friend class Application;
		friend class SceneManager;
	};
}