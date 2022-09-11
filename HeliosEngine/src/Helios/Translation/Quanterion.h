#pragma once

#include "Vector.h"

namespace Helios {
	
	struct Quanterion
	{
        union {
            float q[4];
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };

		Quanterion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { };
		Quanterion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { };
		Quanterion(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };
        Quanterion(const Quanterion& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };

        static float Dot(const Quanterion& lhs, const Quanterion& rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        // Operator check if Quanterion is equal using Dot product


        static const Quanterion zero() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }

        static Quanterion Euler(Vector3 vec) { return EulerRads(vec * ((float)M_PI / 180.0f)); }
        static Quanterion Euler(float x, float y, float z) { return EulerRads(x * ((float)M_PI / 180.0f), y * ((float)M_PI / 180.0f), z * ((float)M_PI / 180.0f)); }
        static Quanterion EulerRads(Vector3 vec) { return EulerRads(vec.x, vec.y, vec.z); }
        // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
        static Quanterion EulerRads(float x, float y, float z)
        {
            // Convert Euler Rotation (radians) to Quanterion Rotation and return it

            float c1 = cosf( x / 2.0f );
		    float c2 = cosf( y / 2.0f );
		    float c3 = cosf( z / 2.0f );

		    float s1 = sinf( x / 2.0f );
		    float s2 = sinf( y / 2.0f );
		    float s3 = sinf( z / 2.0f );

            return {
                s1 * c2 * c3 + c1 * s2 * s3,
                c1 * s2 * c3 - s1 * c2 * s3,
                c1 * c2 * s3 + s1 * s2 * c3,
                c1 * c2 * c3 - s1 * s2 * s3,
            };
        }

        Vector3 euler()
        {
            return { 0.0f, 0.0f, 0.0f };
        }
        
        // Multiply by vector

        Vector3 operator*(const Vector3& rhs) const
        {
            // https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
            // https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm

            // Vector3 qvec = { x, y, z };
            // Vector3 uv = Vector3::Cross(qvec, rhs);
            // Vector3 uuv = Vector3::Cross(qvec, uv);
            // uv *= (2.0f * w);
            // uuv *= 2.0f;

            // return rhs + uv + uuv;

            // https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm

            Vector3 qvec = { x, y, z };
            Vector3 uv = Vector3::Cross(qvec, rhs);
            Vector3 uuv = Vector3::Cross(qvec, uv);
            uv *= (2.0f * w);
            uuv *= 2.0f;

            return rhs + uv + uuv;
        }
	};
}