#include "Quanterion.h"
#include "Matrix.h"

namespace Helios
{
	Vector3 Quanterion::eulerRads() //YXZ
	{
        Matrix4x4 matrix = Matrix4x4::Rotation(*this);

		if (abs(matrix._32) < 0.9999999f) {
			return {
				asin(-std::clamp(matrix._32, -1.0f, 1.0f)),
				atan2(matrix._31, matrix._33),
				atan2(matrix._12, matrix._22)
			};
		}
		else {
			return {
				asin(-std::clamp(matrix._32, -1.0f, 1.0f)),
				atan2(-matrix._13, matrix._11),
				0.0f
			};
		}
	}
}