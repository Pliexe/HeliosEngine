#pragma once

#include "pch.h"
#include "Helios/GameObjects/ObjectComponent.h"
#include "Helios/Resources/Brushes/Brush.h"
#include "Helios/Resources/Brushes/BitmapBrush.h"
#include "Helios/Core/Base.h"

namespace Helios {
	class HELIOS_API EllipseComponent : public ObjectComponent{
	private:

		float stroke = 0.0f;
		ID2D1StrokeStyle* strokeStyle = NULL;
		std::unique_ptr<Brush> m_brush;

		void Render(DepricatedGraphics& g) override {
			if (m_brush->isReady())
			{
				/*auto pos = transform->getScreenPosition();
				if (m_brush->isPainted())
				{
					auto brush = dynamic_cast<BitmapBrush*>(m_brush.get());
					if (brush) {
						auto Size = brush->getSize();
						(*brush)->SetTransform(
							D2D1::Matrix3x2F::Scale(D2D1::SizeF((pos.w - pos.x) / Size.width, (pos.h - pos.y) / Size.height)) *
							D2D1::Matrix3x2F::TranslationRow(D2D1::SizeF(pos.x, pos.y))
						);
					}
				}

				if (stroke <= 0.0f)
					g.getRenderTarget()->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pos.x + transform->getSize().width / 2.0f, pos.y + transform->getSize().height / 2.0f), transform->getSize().width, transform->getSize().height), *m_brush);
				else
					g.getRenderTarget()->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(pos.x + transform->getSize().width / 2.0f, pos.y + transform->getSize().height / 2.0f), transform->getSize().width, transform->getSize().height), *m_brush, stroke);*/
			}
		}



	public:

		EllipseComponent() = delete;
		template <class T = Brush>
		EllipseComponent(T brush, float strokeWidth = 0.0f) {
			static_assert(std::is_base_of<Brush, T>::value, "Type T must be derived from Brush!");
			m_brush = std::make_unique<T>(brush);
			this->stroke = strokeWidth;
		}

		friend class DepricatedApplication;
		friend class SceneManager;
	};
}