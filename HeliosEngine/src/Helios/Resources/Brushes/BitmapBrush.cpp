#include "BitmapBrush.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Resources/Bitmap.h"
#include "Helios/Translation/Vector2D.h"

Helios::BitmapBrush::BitmapBrush(const wchar_t* path)
{
	bitmap = std::make_shared<Bitmap>(path);
	Load();
}

Helios::BitmapBrush::BitmapBrush(Bitmap& img)
{
	if (img.isNull())
		bitmap = nullptr;
	else {
		bitmap = std::make_shared<Bitmap>(img);
		Load();
	}
}

bool Helios::BitmapBrush::isPainted() const
{
	return true;
}

inline Helios::Size2D Helios::BitmapBrush::getSize() const { 
	if (bitmap->isNull()) return { 0, 0 };
	D2D1_SIZE_F s = ((ID2D1Bitmap*)(*bitmap))->GetSize();
	return Size2D(s.width, s.height);
}

void Helios::BitmapBrush::Load()
{
	ID2D1BitmapBrush* tmp = NULL;
	Graphics::instance->m_renderTarget2D->CreateBitmapBrush(*bitmap, &tmp);
	brush.reset(tmp, Brush::releaseBrush);
}

Helios::Brush::BrushType Helios::BitmapBrush::getType() const
{
	return BrushType::Bitmap;
}