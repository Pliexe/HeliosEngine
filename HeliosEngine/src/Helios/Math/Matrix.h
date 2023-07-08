#pragma once

#include "Quaternion.h"
#include "Vector.h"

namespace Helios
{
	__declspec(align(16)) struct Matrix4x4
	{
		union
		{
			float m[4][4];
			float matrix[4 * 4];
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			#if defined(__SSE_ENABLED__)
			__m128 rows[4];
			#endif
		};

		static Matrix4x4 TranslationColumn(float x, float y, float z)
		{
			return {
				1.0f, 0.0f, 0.0f, x,
				0.0f, 1.0f, 0.0f, y,
				0.0f, 0.0f, 1.0f, z,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static Matrix4x4 TranslationColumn(const Vector3& translation)
		{
			return {
				1.0f, 0.0f, 0.0f, translation.x,
				0.0f, 1.0f, 0.0f, translation.y,
				0.0f, 0.0f, 1.0f, translation.z,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static Matrix4x4 TranslationRow(float x, float y, float z)
		{
			return {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x,	  y,    z,    1.0f
			};
		}

		static Matrix4x4 TranslationRow(const Vector3& translation)
		{
			return {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				translation.x, translation.y, translation.z, 1.0f
			};
		}


		static Matrix4x4 RotationXRow(float x)
		{
			return {
				1.0f,  0.0f,   0.0f,   0.0f,
				0.0f,  cos(x), sin(x), 0.0f,
				0.0f, -sin(x), cos(x), 0.0f,
				0.0f,  0.0f,   0.0f,   1.0f
			};
		}

		static Matrix4x4 RotationYRow(float y)
		{
			return {
				cos(y), 0.0f, -sin(y), 0.0f,
				0.0f,   1.0f,  0.0f,   0.0f,
				sin(y), 0.0f,  cos(y), 0.0f,
				0.0f,   0.0f,  0.0f,   1.0f
			};
		}

		static Matrix4x4 RotationZRow(float z)
		{
			return {
				cos(z), -sin(z), 0.0f, 0.0f,
				sin(z),  cos(z), 0.0f, 0.0f,
				0.0f,    0.0f,   1.0f, 0.0f,
				0.0f,    0.0f,   0.0f, 1.0f
			};
		}
		static Matrix4x4 RotationRow(Vector3 vec)
		{
			return RotationXRow(vec.x) * RotationZRow(vec.z) * RotationYRow(vec.y);
		}
		
		static Matrix4x4 RotationRow(Quaternion rotation)
		{
			// Create a 4x4 rotation matrix from a quaternion in row major 

			return {
				1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static Matrix4x4 RotationColumn(Quaternion rotation)
		{
			// Create a 4x4 rotation matrix from a quaternion in column major

			return {
				1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static float Determinant(Matrix4x4 matrix)
		{
			// Calculate the determinant of a 4x4 matrix

			// TODO: Same as inverse, I can't be bothered to do this right now

			return
				matrix._11 * matrix._22 * matrix._33 * matrix._44 +
				matrix._11 * matrix._23 * matrix._34 * matrix._42 +
				matrix._11 * matrix._24 * matrix._32 * matrix._43 +
				matrix._12 * matrix._21 * matrix._34 * matrix._43 +
				matrix._12 * matrix._23 * matrix._31 * matrix._44 +
				matrix._12 * matrix._24 * matrix._33 * matrix._41 +
				matrix._13 * matrix._21 * matrix._32 * matrix._44 +
				matrix._13 * matrix._22 * matrix._34 * matrix._41 +
				matrix._13 * matrix._24 * matrix._31 * matrix._42 +
				matrix._14 * matrix._21 * matrix._33 * matrix._42 +
				matrix._14 * matrix._22 * matrix._31 * matrix._43 +
				matrix._14 * matrix._23 * matrix._32 * matrix._41 -
				matrix._11 * matrix._22 * matrix._34 * matrix._43 -
				matrix._11 * matrix._23 * matrix._32 * matrix._44 -
				matrix._11 * matrix._24 * matrix._33 * matrix._42 -
				matrix._12 * matrix._21 * matrix._33 * matrix._44 -
				matrix._12 * matrix._23 * matrix._34 * matrix._41 -
				matrix._12 * matrix._24 * matrix._31 * matrix._43 -
				matrix._13 * matrix._21 * matrix._34 * matrix._42 -
				matrix._13 * matrix._22 * matrix._31 * matrix._44 -
				matrix._13 * matrix._24 * matrix._32 * matrix._41 -
				matrix._14 * matrix._21 * matrix._32 * matrix._43 -
				matrix._14 * matrix._22 * matrix._33 * matrix._41 -
				matrix._14 * matrix._23 * matrix._31 * matrix._42;
		}

		static Matrix4x4 Inverse(Matrix4x4 matrix)
		{
			Matrix4x4 result = matrix;

			// Calculate the determinant of the matrix

			float determinant = Determinant(matrix);

			// If the determinant is 0, then the matrix cannot be inverted

			if (determinant == 0.0f)
				return Matrix4x4::Identity();

			// Calculate the inverse of the matrix

			// TODO: This can be optimized, there are repeated calculations here but I can't be bothered to optimize it right now since it's a wall of text

			result._11 = (matrix._22 * matrix._33 * matrix._44 + matrix._23 * matrix._34 * matrix._42 + matrix._24 * matrix._32 * matrix._43 - matrix._22 * matrix._34 * matrix._43 - matrix._23 * matrix._32 * matrix._44 - matrix._24 * matrix._33 * matrix._42) / determinant;
			result._12 = (matrix._12 * matrix._34 * matrix._43 + matrix._13 * matrix._32 * matrix._44 + matrix._14 * matrix._33 * matrix._42 - matrix._12 * matrix._33 * matrix._44 - matrix._13 * matrix._34 * matrix._42 - matrix._14 * matrix._32 * matrix._43) / determinant;
			result._13 = (matrix._12 * matrix._23 * matrix._44 + matrix._13 * matrix._24 * matrix._42 + matrix._14 * matrix._22 * matrix._43 - matrix._12 * matrix._24 * matrix._43 - matrix._13 * matrix._22 * matrix._44 - matrix._14 * matrix._23 * matrix._42) / determinant;
			result._14 = (matrix._12 * matrix._24 * matrix._33 + matrix._13 * matrix._22 * matrix._34 + matrix._14 * matrix._23 * matrix._32 - matrix._12 * matrix._23 * matrix._34 - matrix._13 * matrix._24 * matrix._32 - matrix._14 * matrix._22 * matrix._33) / determinant;
			result._21 = (matrix._21 * matrix._34 * matrix._43 + matrix._23 * matrix._31 * matrix._44 + matrix._24 * matrix._33 * matrix._41 - matrix._21 * matrix._33 * matrix._44 - matrix._23 * matrix._34 * matrix._41 - matrix._24 * matrix._31 * matrix._43) / determinant;
			result._22 = (matrix._11 * matrix._33 * matrix._44 + matrix._13 * matrix._34 * matrix._41 + matrix._14 * matrix._31 * matrix._43 - matrix._11 * matrix._34 * matrix._43 - matrix._13 * matrix._31 * matrix._44 - matrix._14 * matrix._33 * matrix._41) / determinant;
			result._23 = (matrix._11 * matrix._24 * matrix._43 + matrix._13 * matrix._21 * matrix._44 + matrix._14 * matrix._23 * matrix._41 - matrix._11 * matrix._23 * matrix._44 - matrix._13 * matrix._24 * matrix._41 - matrix._14 * matrix._21 * matrix._43) / determinant;
			result._24 = (matrix._11 * matrix._23 * matrix._34 + matrix._13 * matrix._24 * matrix._31 + matrix._14 * matrix._21 * matrix._33 - matrix._11 * matrix._24 * matrix._33 - matrix._13 * matrix._21 * matrix._34 - matrix._14 * matrix._23 * matrix._31) / determinant;
			result._31 = (matrix._21 * matrix._32 * matrix._44 + matrix._22 * matrix._34 * matrix._41 + matrix._24 * matrix._31 * matrix._42 - matrix._21 * matrix._34 * matrix._42 - matrix._22 * matrix._31 * matrix._44 - matrix._24 * matrix._32 * matrix._41) / determinant;
			result._32 = (matrix._11 * matrix._34 * matrix._42 + matrix._12 * matrix._31 * matrix._44 + matrix._14 * matrix._32 * matrix._41 - matrix._11 * matrix._32 * matrix._44 - matrix._12 * matrix._34 * matrix._41 - matrix._14 * matrix._31 * matrix._42) / determinant;
			result._33 = (matrix._11 * matrix._22 * matrix._44 + matrix._12 * matrix._24 * matrix._41 + matrix._14 * matrix._21 * matrix._42 - matrix._11 * matrix._24 * matrix._42 - matrix._12 * matrix._21 * matrix._44 - matrix._14 * matrix._22 * matrix._41) / determinant;
			result._34 = (matrix._11 * matrix._24 * matrix._32 + matrix._12 * matrix._21 * matrix._34 + matrix._14 * matrix._22 * matrix._31 - matrix._11 * matrix._22 * matrix._34 - matrix._12 * matrix._24 * matrix._31 - matrix._14 * matrix._21 * matrix._32) / determinant;
			result._41 = (matrix._21 * matrix._33 * matrix._42 + matrix._22 * matrix._31 * matrix._43 + matrix._23 * matrix._32 * matrix._41 - matrix._21 * matrix._32 * matrix._43 - matrix._22 * matrix._33 * matrix._41 - matrix._23 * matrix._31 * matrix._42) / determinant;
			result._42 = (matrix._11 * matrix._32 * matrix._43 + matrix._12 * matrix._33 * matrix._41 + matrix._13 * matrix._31 * matrix._42 - matrix._11 * matrix._33 * matrix._42 - matrix._12 * matrix._31 * matrix._43 - matrix._13 * matrix._32 * matrix._41) / determinant;
			result._43 = (matrix._11 * matrix._23 * matrix._42 + matrix._12 * matrix._21 * matrix._43 + matrix._13 * matrix._22 * matrix._41 - matrix._11 * matrix._22 * matrix._43 - matrix._12 * matrix._23 * matrix._41 - matrix._13 * matrix._21 * matrix._42) / determinant;
			result._44 = (matrix._11 * matrix._22 * matrix._33 + matrix._12 * matrix._23 * matrix._31 + matrix._13 * matrix._21 * matrix._32 - matrix._11 * matrix._23 * matrix._32 - matrix._12 * matrix._21 * matrix._33 - matrix._13 * matrix._22 * matrix._31) / determinant;

			return result;
		}

		static Matrix4x4 Scale(float x, float y, float z)
		{
#if defined(__SSE_ENABLED__)

			Matrix4x4 result;

			result.rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, x);
			result.rows[1] = _mm_set_ps(0.0f, 0.0f, y, 0.0f);
			result.rows[2] = _mm_set_ps(0.0f, z, 0.0f, 0.0f);
			result.rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

			return result;

#else
			return {
				x,	  0.0f, 0.0f, 0.0f,
				0.0f, y,    0.0f, 0.0f,
				0.0f, 0.0f, z	, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
#endif
		}

		static Matrix4x4 Scale(const Vector3& scaling)
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

		static Matrix4x4 Scale(float scaling)
		{
#if defined(__SSE_ENABLED__)

			Matrix4x4 result;

			result.rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, scaling);
			result.rows[1] = _mm_set_ps(0.0f, 0.0f, scaling, 0.0f);
			result.rows[2] = _mm_set_ps(0.0f, scaling, 0.0f, 0.0f);
			result.rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

			return result;

#else
			return {
				scaling,	  0.0f, 0.0f, 0.0f,
				0.0f, scaling,    0.0f, 0.0f,
				0.0f, 0.0f, scaling, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
#endif
		}

		Matrix4x4 operator* (Matrix4x4 b) { return Matrix4x4::Multiply(*this, b); }
		const Matrix4x4 operator*(const Matrix4x4& b) const { return Matrix4x4::Multiply(*this, b); }
		Vector4 operator*(const Vector4& vector4) const;
		std::string toString() const
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2);
			ss << _11 << " " << _12 << " " << _13 << " " << _14 << "\n";
			ss << _21 << " " << _22 << " " << _23 << " " << _24 << "\n";
			ss << _31 << " " << _32 << " " << _33 << " " << _34 << "\n";
			ss << _41 << " " << _42 << " " << _43 << " " << _44 << "\n";
			return ss.str();
		}

		// Column major
		Vector3 GetTranslation() const
		{
			return { _14, _24, _34 };
		}

		Vector3 GetForward() const
		{
			return { _13, _23, _33 };
		}

		static Matrix4x4 Identity()
		{
			static Matrix4x4 identity = {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			return identity;
		}

		static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b)
		{
			Matrix4x4 result;

#if defined(__SSE_ENABLED__)

			__m128 b0 = _mm_load_ps(&b._11);
			__m128 b1 = _mm_load_ps(&b._21);
			__m128 b2 = _mm_load_ps(&b._31);
			__m128 b3 = _mm_load_ps(&b._41);

			// Perform the multiplication using SSE intrinsics
			__m128 r0 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._11), b0),
				_mm_mul_ps(_mm_set1_ps(a._12), b1)),
				_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._13), b2),
					_mm_mul_ps(_mm_set1_ps(a._14), b3)));
			__m128 r1 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._21), b0),
				_mm_mul_ps(_mm_set1_ps(a._22), b1)),
				_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._23), b2),
					_mm_mul_ps(_mm_set1_ps(a._24), b3)));
			__m128 r2 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._31), b0),
				_mm_mul_ps(_mm_set1_ps(a._32), b1)),
				_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._33), b2),
					_mm_mul_ps(_mm_set1_ps(a._34), b3)));
			__m128 r3 = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._41), b0),
				_mm_mul_ps(_mm_set1_ps(a._42), b1)),
				_mm_add_ps(_mm_mul_ps(_mm_set1_ps(a._43), b2),
					_mm_mul_ps(_mm_set1_ps(a._44), b3)));

			// Store the results back into the result matrix
			_mm_store_ps(&result._11, r0);
			_mm_store_ps(&result._21, r1);
			_mm_store_ps(&result._31, r2);
			_mm_store_ps(&result._41, r3);

#else
			
			float x = a._11;
			float y = a._12;
			float z = a._13;
			float w = a._14;

			result._11 = (b._11 * x) + (b._21 * y) + (b._31 * z) + (b._41 * w);
			result._12 = (b._12 * x) + (b._22 * y) + (b._32 * z) + (b._42 * w);
			result._13 = (b._13 * x) + (b._23 * y) + (b._33 * z) + (b._43 * w);
			result._14 = (b._14 * x) + (b._24 * y) + (b._34 * z) + (b._44 * w);

			x = a._21;
			y = a._22;
			z = a._23;
			w = a._24;
			result._21 = (b._11 * x) + (b._21 * y) + (b._31 * z) + (b._41 * w);
			result._22 = (b._12 * x) + (b._22 * y) + (b._32 * z) + (b._42 * w);
			result._23 = (b._13 * x) + (b._23 * y) + (b._33 * z) + (b._43 * w);
			result._24 = (b._14 * x) + (b._24 * y) + (b._34 * z) + (b._44 * w);

			x = a._31;
			y = a._32;
			z = a._33;
			w = a._34;
			result._31 = (b._11 * x) + (b._21 * y) + (b._31 * z) + (b._41 * w);
			result._32 = (b._12 * x) + (b._22 * y) + (b._32 * z) + (b._42 * w);
			result._33 = (b._13 * x) + (b._23 * y) + (b._33 * z) + (b._43 * w);
			result._34 = (b._14 * x) + (b._24 * y) + (b._34 * z) + (b._44 * w);

			x = a._41;
			y = a._42;
			z = a._43;
			w = a._44;
			result._41 = (b._11 * x) + (b._21 * y) + (b._31 * z) + (b._41 * w);
			result._42 = (b._12 * x) + (b._22 * y) + (b._32 * z) + (b._42 * w);
			result._43 = (b._13 * x) + (b._23 * y) + (b._33 * z) + (b._43 * w);
			result._44 = (b._14 * x) + (b._24 * y) + (b._34 * z) + (b._44 * w);

#endif
			return result;
		}

		// Creates a right handed perspective projection matrix in column major order
		static Matrix4x4 PerspectiveRH(float fov, float aspectRatio, float near_c, float far_c)
		{
			float q = 1.0f / tan(fov * 0.5f);
			float a = q / aspectRatio;
			float b = (near_c + far_c) / (near_c - far_c);
			float c = (2.0f * near_c * far_c) / (near_c - far_c);		

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, q, 0.0f, 0.0f,
				0.0f, 0.0f, b, c,
				0.0f, 0.0f, -1.0f, 0.0f
			};
		}

		
		// Creates a left handed perspective projection matrix in column major order
		static Matrix4x4 PerspectiveLH(float fov, float aspectRatio, float near_c, float far_c)
		{
			float q = 1.0f / tan(fov * 0.5f);
			float a = q / aspectRatio;
			float b = far_c / (far_c - near_c);
			float c = (-far_c * near_c) / (far_c - near_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, q, 0.0f, 0.0f,
				0.0f, 0.0f, b, 	c,
				0.0f, 0.0f, 1.0f, 0.0f
			};
		}

		static Matrix4x4 PerspectiveLHRow(float fov, float aspect_ratio, float near_c, float far_c)
		{
			float q = 1.0f / tan(fov * 0.5f);
			float a = q / aspect_ratio;
			float b = far_c / (far_c - near_c);
			float c = (-far_c * near_c) / (far_c - near_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, q, 0.0f, 0.0f,
				0.0f, 0.0f, b, 0.0f,
				0.0f, 0.0f, c, 1.0f
			};
		}

		// Creates a right handed orthographic projection matrix in column major order
		static Matrix4x4 OrthographicRH(float width, float height, float near_c, float far_c)
		{
			float a = 2.0f / width;
			float b = 2.0f / height;
			float c = 1.0f / (near_c - far_c);
			float d = near_c / (near_c - far_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, b, 0.0f, 0.0f,
				0.0f, 0.0f, c, d,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		// Creates a left handed orthographic projection matrix in column major order
		static Matrix4x4 OrthographicLH(float size, float aspect_ratio, float near_c, float far_c)
		{
			float a = 2.0f / size;
			float b = 2.0f / (size / aspect_ratio);
			float c = 1.0f / (far_c - near_c);
			float d = near_c / (near_c - far_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, b, 0.0f, 0.0f,
				0.0f, 0.0f, c, d,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static Matrix4x4 Transpose(const Matrix4x4& matrix) 
		{
#if defined(__SSE_ENABLED__)
			
			__m128 row1 = _mm_loadu_ps(&matrix._11);
			__m128 row2 = _mm_loadu_ps(&matrix._21);
			__m128 row3 = _mm_loadu_ps(&matrix._31);
			__m128 row4 = _mm_loadu_ps(&matrix._41);

			__m128 t0 = _mm_unpacklo_ps(row1, row2);
			__m128 t1 = _mm_unpacklo_ps(row3, row4);
			__m128 t2 = _mm_unpackhi_ps(row1, row2);
			__m128 t3 = _mm_unpackhi_ps(row3, row4);

			row1 = _mm_movelh_ps(t0, t1);
			row2 = _mm_movehl_ps(t1, t0);
			row3 = _mm_movelh_ps(t2, t3);
			row4 = _mm_movehl_ps(t3, t2);

			Matrix4x4 result;
			_mm_storeu_ps(&result._11, row1);
			_mm_storeu_ps(&result._21, row2);
			_mm_storeu_ps(&result._31, row3);
			_mm_storeu_ps(&result._41, row4);

			return result;

#else
			return {
				matrix._11, matrix._21, matrix._31, matrix._41,
				matrix._12, matrix._22, matrix._32, matrix._42,
				matrix._13, matrix._23, matrix._33, matrix._43,
				matrix._14, matrix._24, matrix._34, matrix._44
			};
#endif
		}

#define Translation TranslationColumn
#define Rotation RotationColumn
	};

	inline Vector4 Matrix4x4::operator*(const Vector4& vector4) const
	{
		Vector4 result;
		
		#if defined(__SSE_ENABLED__)

		__m128 vec4row = _mm_set_ps(vector4.w, vector4.z, vector4.y, vector4.x);
		
		result.x = _mm_dp_ps(rows[0], vec4row, 0xFF).m128_f32[0];
		result.y = _mm_dp_ps(rows[1], vec4row, 0xFF).m128_f32[0];
		result.z = _mm_dp_ps(rows[2], vec4row, 0xFF).m128_f32[0];
		result.w = _mm_dp_ps(rows[3], vec4row, 0xFF).m128_f32[0];

		#else

		result.x = _11 * vector4.x + _12 * vector4.y + _13 * vector4.z + _14 * vector4.w;
		result.y = _21 * vector4.x + _22 * vector4.y + _23 * vector4.z + _24 * vector4.w;
		result.z = _31 * vector4.x + _32 * vector4.y + _33 * vector4.z + _34 * vector4.w;
		result.w = _41 * vector4.x + _42 * vector4.y + _43 * vector4.z + _44 * vector4.w;
		
		#endif

		return result;
	}
}	
