#include "BitmapBrush.h"
#include "Helios/Graphics/DepricatedGraphics.h"
#include "Helios/Resources/Bitmap.h"
#include "Helios/Translation/Vector.h"

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

inline Helios::Size Helios::BitmapBrush::getSize() const { 
	if (bitmap->isNull()) return { 0, 0 };
	D2D1_SIZE_F s = ((ID2D1Bitmap*)(*bitmap))->GetSize();
	return Size(s.width, s.height);
}

void Helios::BitmapBrush::Load()
{
	ID2D1BitmapBrush* tmp = NULL;
	DepricatedGraphics::instance->m_renderTarget2D->CreateBitmapBrush(*bitmap, &tmp);
	brush.reset(tmp, Brush::releaseBrush);
}

Helios::BrushType Helios::BitmapBrush::getType() const
{
	return BrushType::Bitmap;
}