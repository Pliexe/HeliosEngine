#pragma once

#include "Quanterion.h"

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


		static Matrix4x4 RotationX(float x)
		{
			return {
				1.0f,  0.0f,   0.0f,   0.0f,
				0.0f,  cos(x), sin(x), 0.0f,
				0.0f, -sin(x), cos(x), 0.0f,
				0.0f,  0.0f,   0.0f,   1.0f
			};
		}

		static Matrix4x4 RotationY(float y)
		{
			return {
				cos(y), 0.0f, -sin(y), 0.0f,
				0.0f,   1.0f,  0.0f,   0.0f,
				sin(y), 0.0f,  cos(y), 0.0f,
				0.0f,   0.0f,  0.0f,   1.0f
			};
		}

		static Matrix4x4 RotationZ(float z)
		{
			return {
				cos(z), -sin(z), 0.0f, 0.0f,
				sin(z),  cos(z), 0.0f, 0.0f,
				0.0f,    0.0f,   1.0f, 0.0f,
				0.0f,    0.0f,   0.0f, 1.0f
			};
		}
		static Matrix4x4 Rotation(Vector3 vec)
		{
			return RotationZ(vec.z) * RotationY(vec.y) * RotationX(vec.x);
		}
		
		static Matrix4x4 Rotation(Quanterion rotation)
		{

			// Create a 4x4 rotation matrix from a quaternion

			/*return {
				1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};*/


			// Create a 4x4 rotation matrix from a quaternion in row major 

			return {
				1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 0.0f,
				2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};


			// return {
			// 	1.0f - 2.0f * (rotation.y * rotation.y + rotation.z * rotation.z), 2.0f * (rotation.x * rotation.y + rotation.z * rotation.w), 2.0f * (rotation.x * rotation.z - rotation.y * rotation.w), 0.0f,
			// 	2.0f * (rotation.x * rotation.y - rotation.z * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.z * rotation.z), 2.0f * (rotation.y * rotation.z + rotation.x * rotation.w), 0.0f,
			// 	2.0f * (rotation.x * rotation.z + rotation.y * rotation.w), 2.0f * (rotation.y * rotation.z - rotation.x * rotation.w), 1.0f - 2.0f * (rotation.x * rotation.x + rotation.y * rotation.y), 0.0f,
			// 	0.0f, 0.0f, 0.0f, 1.0f
			// };

			// https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
			
			/*return {
				rotation.q[0] * rotation.q[0] + rotation.q[1] * rotation.q[1] - rotation.q[2] * rotation.q[2] - rotation.q[3] * rotation.q[3],2 * rotation.q[1] * rotation.q[2] - 2 * rotation.q[0] * rotation.q[3], 2 * rotation.q[1] * rotation.q[3] + 2 * rotation.q[0] * rotation.q[2], 0.0f,
				2 * rotation.q[1] * rotation.q[2] + 2 * rotation.q[0] * rotation.q[3], rotation.q[0] * rotation.q[0] - rotation.q[1] * rotation.q[1] + rotation.q[2] * rotation.q[2] - rotation.q[3] * rotation.q[3], 2 * rotation.q[2] * rotation.q[3] - 2 * rotation.q[0] * rotation.q[1], 0.0f,
				2 * rotation.q[1] * rotation.q[3] - 2 * rotation.q[0] * rotation.q[2], 2 * rotation.q[2] * rotation.q[3] + 2 * rotation.q[0] * rotation.q[1], rotation.q[0] * rotation.q[0] - rotation.q[1] * rotation.q[1] - rotation.q[2] * rotation.q[2] + rotation.q[3] * rotation.q[3], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};*/
		}

		static Matrix4x4 Translation(float x, float y, float z)
		{
			return {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x,	  y,    z,    1.0f
			};
		}

		static Matrix4x4 Translation(const Vector3& translation)
		{
			return {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				translation.x, translation.y, translation.z, 1.0f
			};
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

		Matrix4x4 operator* (Matrix4x4 b) { return Matrix4x4::Multiply(*this, b); }

		static Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b)
		{
			return {
				/*c._11 = */a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41,
				/*c._12 = */a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42,
				/*c._13 = */a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43,
				/*c._14 = */a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44,

				/*c._21 = */a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41,
				/*c._22 = */a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42,
				/*c._23 = */a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43,
				/*c._24 = */a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44,

				/*c._31 = */a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41,
				/*c._32 = */a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42,
				/*c._33 = */a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43,
				/*c._34 = */a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44,

				/*c._41 = */a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41,
				/*c._42 = */a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42,
				/*c._43 = */a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43,
				/*c._44 = */a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44,
			};
		}

		static Matrix4x4 PerspectiveRH(float fov, float aspectRatio, float near_c, float far_c)
		{
			float q = 1.0f / tan(fov * 0.5f);
			float a = q / aspectRatio;
			float b = (near_c + far_c) / (near_c - far_c);
			float c = (2.0f * near_c * far_c) / (near_c - far_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, q, 0.0f, 0.0f,
				0.0f, 0.0f, b, -1.0f,
				0.0f, 0.0f, c, 0.0f
			};
		}

		static Matrix4x4 PerspectiveLH(float fov, float aspectRatio, float near_c, float far_c)
		{
			float q = 1.0f / tan(fov * 0.5f);
			float a = q / aspectRatio;
			float b = far_c / (far_c - near_c);
			float c = (-far_c * near_c) / (far_c - near_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, q, 0.0f, 0.0f,
				0.0f, 0.0f, b, 1.0f,
				0.0f, 0.0f, c, 0.0f
			};
		}

		static Matrix4x4 OrthographicRH(float width, float height, float near_c, float far_c)
		{
			float a = 2.0f / width;
			float b = 2.0f / height;
			float c = 1.0f / (near_c - far_c);
			float d = near_c / (near_c - far_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, b, 0.0f, 0.0f,
				0.0f, 0.0f, c, 0.0f,
				0.0f, 0.0f, d, 1.0f
			};
		}

		static Matrix4x4 OrthographicLH(float width, float height, float near_c, float far_c)
		{
			float a = 2.0f / width;
			float b = 2.0f / height;
			float c = 1.0f / (far_c - near_c);
			float d = near_c / (near_c - far_c);

			return {
				a, 0.0f, 0.0f, 0.0f,
				0.0f, b, 0.0f, 0.0f,
				0.0f, 0.0f, c, 0.0f,
				0.0f, 0.0f, d, 1.0f
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
	};
}