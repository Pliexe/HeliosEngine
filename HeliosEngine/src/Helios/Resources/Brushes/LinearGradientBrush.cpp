#include "LinearGradientBrush.h"
#include "Helios/Resources/Color.h"
#include "Helios/Graphics/Graphics.h"

//Helios::LinearGradientBrush::LinearGradientBrush(const std::initializer_list<GradientStop> stops)
//{
//	D2D1_GRADIENT_STOP* gradientStops = new D2D1_GRADIENT_STOP[stops.size()];
//	int i = 0;
//	for (auto& stop : stops)
//	{
//		gradientStops[i].color = ((D2D1_COLOR_F)((Helios::Color)stop.color));
//		gradientStops[i].position = stop.position;
//		i++;
//	}
//	ID2D1GradientStopCollection* tmp;
//	if (SUCCEEDED(Graphics::instance->m_renderTarget2D->CreateGradientStopCollection(
//		gradientStops,
//		2,
//		D2D1_GAMMA_2_2,
//		D2D1_EXTEND_MODE_CLAMP,
//		&tmp
//	))) {
//		//m_gradientStops.reset(tmp, [](ID2D1GradientStopCollection* ptr) { ptr->Release(); });
//
//		ID2D1LinearGradientBrush* tmp2 = nullptr;
//		Graphics::instance->m_renderTarget2D->CreateLinearGradientBrush(
//			D2D1::LinearGradientBrushProperties(
//				D2D1::Point2F(0, 0),
//				D2D1::Point2F(150, 150)),
//			tmp,
//			&tmp2
//		);
//		brush.reset(tmp2, Brush::releaseBrush);
//	}
//	delete[] gradientStops;
//}
//
//bool Helios::LinearGradientBrush::isPainted() const { return false; }
