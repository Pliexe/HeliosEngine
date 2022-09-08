#include "SolidBrush.h"
#include "Helios/Graphics/Graphics.h"

Helios::SolidBrush::SolidBrush(Color color)
{
	ID2D1SolidColorBrush* tmp = NULL;
	Graphics::instance->m_renderTarget2D->CreateSolidColorBrush(color, &tmp);
	brush.reset(tmp, [](ID2D1SolidColorBrush* ptr) {
		ptr->Release();
	});
}

Helios::SolidBrush::SolidBrush(float r, float g, float b, float a)
{
	ID2D1SolidColorBrush* tmp = NULL;
	Graphics::instance->m_renderTarget2D->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &tmp);
	brush.reset(tmp, [](ID2D1SolidColorBrush* ptr) {
		ptr->Release();
	});
}

bool Helios::SolidBrush::isPainted() const
{
	return false;
}

Helios::BrushType Helios::SolidBrush::getType() const
{
	return BrushType::Solid;
}