#pragma once

#include "pch.h"

#include <Helios/Math/Vector.h>
#include <Helios/Math/Quaternion.h>

namespace ImGui {
	
	bool EditVector2(const char* label, float width, Helios::Vector2& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
	bool EditVector3(const char* label, float width, Helios::Vector3& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void EditQuanterionEuler(const char* label, float width, Helios::Quaternion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void EditQuanterion(const char* label, Helios::Quaternion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
}
