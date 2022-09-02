#include "SolidBrush.h"
#include "Wallnut/Graphics/Graphics.h"

Wallnut::SolidBrush::SolidBrush(Color color)
{
	ID2D1SolidColorBrush* tmp = NULL;
	Graphics::instance->renderTarget->CreateSolidColorBrush(color, &tmp);
	brush.reset(tmp, [](ID2D1SolidColorBrush* ptr) {
		ptr->Release();
	});
}

Wallnut::SolidBrush::SolidBrush(float r, float g, float b, float a)
{
	ID2D1SolidColorBrush* tmp = NULL;
	Graphics::instance->renderTarget->CreateSolidColorBrush(D2D1::ColorF(r, g, b, a), &tmp);
	brush.reset(tmp, [](ID2D1SolidColorBrush* ptr) {
		ptr->Release();
	});
}

bool Wallnut::SolidBrush::isPainted() const
{
	return false;
}

Wallnut::Brush::BrushType Wallnut::SolidBrush::getType() const
{
	return BrushType::Solid;
}