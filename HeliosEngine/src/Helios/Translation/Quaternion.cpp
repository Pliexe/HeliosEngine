#include "Quaternion.h"
#include "Matrix.h"

namespace Helios
{
	Vector3 Quaternion::eulerRads() //YXZ
	{
        Matrix4x4 matrix = Matrix4x4::RotationRow(*this);

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

	Quaternion Quaternion::operator*(const Quaternion& rotation) const
	{
		return {
			w * rotation.x + x * rotation.w + y * rotation.z - z * rotation.y,
			w * rotation.y + y * rotation.w + z * rotation.x - x * rotation.z,
			w * rotation.z + z * rotation.w + x * rotation.y - y * rotation.x,
			w * rotation.w - x * rotation.x - y * rotation.y - z * rotation.z
		};
	}
}
