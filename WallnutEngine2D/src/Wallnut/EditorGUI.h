#pragma once

#include "pch.h"
#include "Core.h"

namespace Wallnut {
	class Vector2D;
	class SolidBrush;
}
namespace WallnutEditor {
	class WALLNUT_API EditorGUI
	{
	public:
		static void Color(ID2D1SolidColorBrush* brush);
		static void Color(Wallnut::SolidBrush& brush);

		static void DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max);
		
		static void DragVector2D(const char* label, Wallnut::Vector2D* vec);
		static void DragVector2D(const char* label, Wallnut::Vector2D& vec);
	};
}