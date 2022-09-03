#include "BitmapBrush.h"
#include "Wallnut/Graphics/Graphics.h"
#include "Wallnut/Resources/Bitmap.h"
#include "Wallnut/Translation/Vector2D.h"

Wallnut::BitmapBrush::BitmapBrush(const wchar_t* path)
{
	bitmap = std::make_shared<Bitmap>(path);
	Load();
}

Wallnut::BitmapBrush::BitmapBrush(Bitmap& img)
{
	if (img.isNull())
		bitmap = nullptr;
	else {
		bitmap = std::make_shared<Bitmap>(img);
		Load();
	}
}

bool Wallnut::BitmapBrush::isPainted() const
{
	return true;
}

inline Wallnut::Size2D Wallnut::BitmapBrush::getSize() const { 
	if (bitmap->isNull()) return { 0, 0 };
	D2D1_SIZE_F s = ((ID2D1Bitmap*)(*bitmap))->GetSize();
	return Size2D(s.width, s.height);
}

void Wallnut::BitmapBrush::Load()
{
	ID2D1BitmapBrush* tmp = NULL;
	Graphics::instance->m_renderTarget2D->CreateBitmapBrush(*bitmap, &tmp);
	brush.reset(tmp, Brush::releaseBrush);
}

Wallnut::Brush::BrushType Wallnut::BitmapBrush::getType() const
{
	return BrushType::Bitmap;
}