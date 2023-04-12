#pragma once

#include "Vector.h"

namespace Helios {
	struct HELIOS_API Quaternion
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

		Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { };
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { };
		Quaternion(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };
        Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };

        static float Dot(const Quaternion& lhs, const Quaternion& rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        // Operator check if Quaternion is equal using Dot product


        static const Quaternion Identity() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

        static Quaternion FromEuler(Vector3 vec) { return FromEulerRads(vec * ((float)M_PI / 180.0f)); }
        static Quaternion FromEuler(float x, float y, float z) { return FromEulerRads(x * ((float)M_PI / 180.0f), y * ((float)M_PI / 180.0f), z * ((float)M_PI / 180.0f)); }
        static Quaternion FromEulerRads(Vector3 vec) { return FromEulerRads(vec.x, vec.y, vec.z); }
        // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
        static Quaternion FromEulerRads(float x, float y, float z) // YXZ
        {
            // Convert Euler RotationRow (radians) to Quaternion RotationRow and return it

            float c1 = cosf( x / 2.0f );
		    float c2 = cosf( y / 2.0f );
		    float c3 = cosf( z / 2.0f );

		    float s1 = sinf( x / 2.0f );
		    float s2 = sinf( y / 2.0f );
		    float s3 = sinf( z / 2.0f );

            return {
                s1 * c2 * c3 + c1 * s2 * s3,
                c1 * s2 * c3 - s1 * c2 * s3,
                c1 * c2 * s3 - s1 * s2 * c3,
                c1 * c2 * c3 + s1 * s2 * s3,
            };
        }

		//static Quaternion Invert(const Quaternion& quat) { return Conjugate(quat); }
		static Quaternion Invert(Quaternion quat) { return Conjugate(quat); }
		static Quaternion Inverse(Quaternion quat) { return Conjugate(quat); }
		
        static Quaternion Conjugate(const Quaternion& quat)
        {
            return { -quat.x, -quat.y, -quat.z, quat.w };
        }

        static Quaternion Conjugate(Quaternion& quat)
        {
            return { -quat.x, -quat.y, -quat.z, quat.w };
        }

        Vector3 eulerRads();
        Vector3 euler() { return eulerRads() * (180.0f / (float)M_PI); }

        Quaternion operator/(float other) const
        {
            return { x / other, y / other, z / other, w / other };
        }

        Quaternion operator*(const Quaternion& rhs)
        {
            return {
                this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y,
                this->w * rhs.y + this->y * rhs.w + this->z * rhs.x - this->x * rhs.z,
                this->w * rhs.z + this->z * rhs.w + this->x * rhs.y - this->y * rhs.x,
                this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z
            };
        }

        Quaternion operator*= (const Quaternion& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        Quaternion operator * (float n)
        {
			return { x * n, y * n, z * n, w * n };
        }

        Vector3 operator*(Vector3 point)
        {
            // Multiply by XYZ direction

            Quaternion q = *this * Quaternion(point.x, point.y, point.z, 0.0f) * Quaternion::Conjugate(*this);
            return { q.x, q.y, q.z };
        }

        Vector3 forward()
        {
            return {
                2 * (x * z + w * y),
                2 * (y * z - w * x),
                1 - 2 * (x * x + y * y)
            };
        }

        Vector3 right()
        {
            return {
                1 - 2 * (y * y + z * z),
                2 * (x * y + w * z),
                2 * (x * z - w * y)
            };
        }

        Vector3 up()
        {
	        return {
                2 * (x * y - w * z),
                1 - 2 * (x * x + z * z),
                2 * (y * z + w * x)
            };
        }

        Quaternion operator*(const Quaternion& rotation) const;

        static Quaternion FromAxisAngle(const Vector3& axis, float angle)
        {
			float halfAngle = angle * 0.5f;
			float sinHalfAngle = sinf(halfAngle);
			float cosHalfAngle = cosf(halfAngle);

			return {
				axis.x * sinHalfAngle,
				axis.y * sinHalfAngle,
				axis.z * sinHalfAngle,
				cosHalfAngle
			};
        }

        // void Rotate(const Vector3& vec)
        // {
        //     *this = Euler(vec) * *this;
        // }
	};
}
