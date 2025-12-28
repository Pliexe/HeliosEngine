#pragma once

#include "Quaternion.h"
#include "Vector.h"

namespace Helios
{
	inline Matrix4x4 Matrix4x4::TranslationColumn(const FVector3& translation)
	{
		return {
			1.0f, 0.0f, 0.0f, translation.x,
			0.0f, 1.0f, 0.0f, translation.y,
			0.0f, 0.0f, 1.0f, translation.z,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline Matrix4x4 Matrix4x4::Translation(const FVector3& translation) { return TranslationColumn(translation); }
	
	inline Matrix4x4 Matrix4x4::TranslationRow(const FVector3& translation)
	{
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			translation.x, translation.y, translation.z, 1.0f
		};
	}

	inline Matrix4x4 Matrix4x4::RotationRow(FVector3 vec)
	{
		return RotationXRow(vec.x) * RotationZRow(vec.z) * RotationYRow(vec.y);
	}
	
	inline Matrix4x4 Matrix4x4::RotationRow(Quaternion rotation)
	{
		// Create a 4x4 rotation matrix from a quaternion in row major 

		return {
			1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 0.0f,
			2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 0.0f,
			2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline Matrix4x4 Matrix4x4::RotationColumn(const Quaternion& rotation)
	{
		// Create a 4x4 rotation matrix from a quaternion in column major

		return {
			1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 0.0f,
			2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 0.0f,
			2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	inline Matrix4x4 Matrix4x4::Scale(const FVector3& scaling)
	{
#if defined(__SSE_ENABLED__)

		Matrix4x4 result;

		// I genuinely have no idea if this is faster than the non-SSE version

		result.rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, scaling.x);
		result.rows[1] = _mm_set_ps(0.0f, 0.0f, scaling.y, 0.0f);
		result.rows[2] = _mm_set_ps(0.0f, scaling.z, 0.0f, 0.0f);
		result.rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
		
		return result;

#else
		return {
			scaling.x,	  0.0f, 0.0f, 0.0f,
			0.0f, scaling.y,    0.0f, 0.0f,
			0.0f, 0.0f, scaling.z	, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		}; 
#endif
	}

	inline FVector3 Matrix4x4::GetTranslation() const
	{
		return { _14, _24, _34 };
	}

	inline FVector3 Matrix4x4::GetForward() const
	{
		return { _13, _23, _33 };
	}

	inline Matrix4x4 Matrix4x4::LookAtLH(const FVector3& position, const FVector3& target, const FVector3& up)
	{
		FVector3 f = (target - position).normalized();
		FVector3 r = (FVector3::Cross(f, up)).normalized();
		FVector3 u = FVector3::Cross(r, f);

		Matrix4x4 lookAtMatrix;
		lookAtMatrix._11 = -r.x; lookAtMatrix._12 = -r.y; lookAtMatrix._13 = -r.z; lookAtMatrix._14 = 0;
		lookAtMatrix._21 = u.x; lookAtMatrix._22 = u.y; lookAtMatrix._23 = u.z; lookAtMatrix._24 = 0;
		lookAtMatrix._31 = f.x; lookAtMatrix._32 = f.y; lookAtMatrix._33 = f.z; lookAtMatrix._34 = 0;
		lookAtMatrix._41 = -Vector3::Dot(r, position); lookAtMatrix._42 = -Vector3::Dot(u, position); lookAtMatrix._43 = -Vector3::Dot(f, position); lookAtMatrix._44 = 1;

		return lookAtMatrix;
	}

	inline Vector4 Matrix4x4::operator*(const Vector4& vector4) const
	{
		Vector4 result;

#if defined(__SSE4_1__)

		__m128 vec4row = _mm_set_ps(vector4.w, vector4.z, vector4.y, vector4.x);

		float temp;
		temp = _mm_cvtss_f32(_mm_dp_ps(rows[0], vec4row, 0xFF));
		_mm_store_ss(&result.x, _mm_set_ss(temp));
		temp = _mm_cvtss_f32(_mm_dp_ps(rows[1], vec4row, 0xFF));
		_mm_store_ss(&result.y, _mm_set_ss(temp));
		temp = _mm_cvtss_f32(_mm_dp_ps(rows[2], vec4row, 0xFF));
		_mm_store_ss(&result.z, _mm_set_ss(temp));
		temp = _mm_cvtss_f32(_mm_dp_ps(rows[3], vec4row, 0xFF));
		_mm_store_ss(&result.w, _mm_set_ss(temp));

#elif defined(__SSE_ENABLED__)

		__m128 vec4row = _mm_set_ps(vector4.w, vector4.z, vector4.y, vector4.x);

		__m128 temp1 = _mm_mul_ps(rows[0], vec4row);
		__m128 temp2 = _mm_mul_ps(rows[1], vec4row);
		__m128 temp3 = _mm_mul_ps(rows[2], vec4row);
		__m128 temp4 = _mm_mul_ps(rows[3], vec4row);

		temp1 = _mm_add_ps(temp1, _mm_movehl_ps(temp1, temp1));
		temp1 = _mm_add_ps(temp1, _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2, 3, 0, 1)));
		result.x = _mm_cvtss_f32(temp1);

		temp2 = _mm_add_ps(temp2, _mm_movehl_ps(temp2, temp2));
		temp2 = _mm_add_ps(temp2, _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(2, 3, 0, 1)));
		result.y = _mm_cvtss_f32(temp2);

		temp3 = _mm_add_ps(temp3, _mm_movehl_ps(temp3, temp3));
		temp3 = _mm_add_ps(temp3, _mm_shuffle_ps(temp3, temp3, _MM_SHUFFLE(2, 3, 0, 1)));
		result.z = _mm_cvtss_f32(temp3);

		temp4 = _mm_add_ps(temp4, _mm_movehl_ps(temp4, temp4));
		temp4 = _mm_add_ps(temp4, _mm_shuffle_ps(temp4, temp4, _MM_SHUFFLE(2, 3, 0, 1)));
		result.w = _mm_cvtss_f32(temp4);

#else

		result.x = _11 * vector4.x + _12 * vector4.y + _13 * vector4.z + _14 * vector4.w;
		result.y = _21 * vector4.x + _22 * vector4.y + _23 * vector4.z + _24 * vector4.w;
		result.z = _31 * vector4.x + _32 * vector4.y + _33 * vector4.z + _34 * vector4.w;
		result.w = _41 * vector4.x + _42 * vector4.y + _43 * vector4.z + _44 * vector4.w;

#endif

		return result;
	}
}