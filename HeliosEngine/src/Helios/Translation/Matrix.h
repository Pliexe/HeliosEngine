#pragma once

#include "Quaternion.h"
#include "Vector.h"

namespace Helios
{
	struct Matrix4x4
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
			return {
				x,	  0.0f, 0.0f, 0.0f,
				0.0f, y,    0.0f, 0.0f,
				0.0f, 0.0f, z	, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static Matrix4x4 Scale(const Vector3& scaling)
		{
			return {
				scaling.x,	  0.0f, 0.0f, 0.0f,
				0.0f, scaling.y,    0.0f, 0.0f,
				0.0f, 0.0f, scaling.z	, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			}; 
		}

		static Matrix4x4 Scale(float scaling)
		{
			return {
				scaling,	  0.0f, 0.0f, 0.0f,
				0.0f, scaling,    0.0f, 0.0f,
				0.0f, 0.0f, scaling, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			}; 
		}

		Matrix4x4 operator* (Matrix4x4 b) { return Matrix4x4::Multiply(*this, b); }
		const Matrix4x4 operator*(const Matrix4x4& b) const { return Matrix4x4::Multiply(*this, b); }
		Vector4 operator*(const Vector4& vector4) const;

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
			return {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b)
		{
			Matrix4x4 result;

			result._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41;
			result._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42;
			result._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43;
			result._14 = a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44;

			result._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41;
			result._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42;
			result._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43;
			result._24 = a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44;

			result._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41;
			result._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42;
			result._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43;
			result._34 = a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44;

			result._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41;
			result._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42;
			result._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43;
			result._44 = a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44;

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
			return {
				matrix._11, matrix._21, matrix._31, matrix._41,
				matrix._12, matrix._22, matrix._32, matrix._42,
				matrix._13, matrix._23, matrix._33, matrix._43,
				matrix._14, matrix._24, matrix._34, matrix._44
			};
		}

#define Translation TranslationColumn
#define Rotation RotationColumn
	};

	inline Vector4 Matrix4x4::operator*(const Vector4& vector4) const
	{
		return {
			_11 * vector4.x + _12 * vector4.y + _13 * vector4.z + _14 * vector4.w,
			_21 * vector4.x + _22 * vector4.y + _23 * vector4.z + _24 * vector4.w,
			_31 * vector4.x + _32 * vector4.y + _33 * vector4.z + _34 * vector4.w,
			_41 * vector4.x + _42 * vector4.y + _43 * vector4.z + _44 * vector4.w
		};
	}
}	
