#pragma once

#include "Brush.h"
#include "Helios/Core/Base.h"
#include "Helios/Translation/Vector.h"
 
namespace Helios {
	//class Size;
	class Bitmap;
	class Transform2D;
	class HELIOS_API BitmapBrush : public Brush {
	private:
		std::shared_ptr<Bitmap> bitmap;
		void Load();

		//Transform2DComponent* transform;
	public:
		BitmapBrush() = delete;
		BitmapBrush(Bitmap& image);
		BitmapBrush(const wchar_t* path);

		inline Size getSize() const;



		// Inherited via Brush
		virtual bool isPainted() const override;
		virtual BrushType getType() const override;
	};
}