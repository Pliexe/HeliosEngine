#pragma once

#include "pch.h"

#include <Helios/Translation/Vector.h>
#include <Helios/Translation/Quaternion.h>

namespace ImGui {
	
	void EditVector3(const char* label, Helios::Vector3& vec, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void EditQuanterionEuler(const char* label, Helios::Quaternion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f);

	void EditQuanterion(const char* label, Helios::Quaternion& quanterion, float speed = 0.1f, float min = 0.0f, float max = 0.0f);
}
