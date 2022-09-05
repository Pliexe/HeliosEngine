#pragma once

#include "Brush.h"
#include "Helios/Core.h"
#include "Helios/Translation/Vector2D.h"
 
namespace Helios {
	//class Size2D;
	class Bitmap;
	class Transform;
	class HELIOS_API BitmapBrush : public Brush {
	private:
		std::shared_ptr<Bitmap> bitmap;
		void Load();

		//Transform* transform;
	public:
		BitmapBrush() = delete;
		BitmapBrush(Bitmap& image);
		BitmapBrush(const wchar_t* path);

		inline Size2D getSize() const;



		// Inherited via Brush
		virtual bool isPainted() const override;
		virtual BrushType getType() const override;
	};
}