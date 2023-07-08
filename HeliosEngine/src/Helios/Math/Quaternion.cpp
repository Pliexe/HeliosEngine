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

	Quaternion Quaternion::FromMatrix(const Matrix4x4& projection)
	{
		// Syntax is _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44

		float trace = projection._11 + projection._22 + projection._33;

		if (trace > 0.0f)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);
			return {
				(projection._32 - projection._23) * s,
				(projection._13 - projection._31) * s,
				(projection._21 - projection._12) * s,
				0.25f / s
			};
		}
		else if (projection._11 > projection._22 && projection._11 > projection._33)
		{

			float s = 2.0f * sqrtf(1.0f + projection._11 - projection._22 - projection._33);
			return {
				0.25f * s,
				(projection._12 + projection._21) / s,
				(projection._13 + projection._31) / s,
				(projection._32 - projection._23) / s
			};
		}
		else if (projection._22 > projection._33)
		{
			float s = 2.0f * sqrtf(1.0f + projection._22 - projection._11 - projection._33);
			return {
				(projection._12 + projection._21) / s,
				0.25f * s,
				(projection._23 + projection._32) / s,
				(projection._13 - projection._31) / s
			};
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + projection._33 - projection._11 - projection._22);
			return {
				(projection._13 + projection._31) / s,
				(projection._23 + projection._32) / s,
				0.25f * s,
				(projection._21 - projection._12) / s
			};
		}
	}
}
