/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/GameObjects/ObjectComponent.h"
#include "Wallnut/Resources/Brushes/Brush.h"
#include "Wallnut/Resources/Brushes/BitmapBrush.h"
#include "Wallnut/Resources/Brushes/Brush.h"

#ifdef WALLNUT_EDITOR
#include "Wallnut/EditorGUI.h"
#endif

namespace Wallnut {
	class RectangleComponent : public ObjectComponent {
	private:

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
							D2D1::Matrix3x2F::Scale(D2D1::SizeF((pos.w - pos.x) / size.width(), (pos.h - pos.y) / size.height())) *
							D2D1::Matrix3x2F::Translation(D2D1::SizeF(pos.x, pos.y))
						);
					}
				}

				if (stroke <= 0.0f)
					g.getRenderTarget()->FillRectangle(pos, *m_brush);
				else
					g.getRenderTarget()->DrawRectangle(pos, *m_brush, stroke);
			}
		}

	public:
		  
		RectangleComponent() = delete;
		template <class T = Brush>
		RectangleComponent(T brush, float strokeWidth = 0.0f) {
			static_assert(std::is_base_of<Brush, T>::value, "Type T must be derived from Brush!");
			m_brush = std::make_unique<T>(brush);
			this->stroke = strokeWidth;
		}

		friend class Application;
		friend class SceneManager;

		extern friend class EditorRectangleComponent;
	};
}