#pragma once

#include "Vector.h"
#include "Helios/Core/Base.h"

namespace Helios {
	struct HELIOS_API Quanterion
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

		Quanterion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { };
		Quanterion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { };
		Quanterion(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };
        Quanterion(const Quanterion& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };

        static float Dot(const Quanterion& lhs, const Quanterion& rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        // Operator check if Quanterion is equal using Dot product


        static const Quanterion Identity() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

        static Quanterion Euler(Vector3 vec) { return EulerRads(vec * ((float)M_PI / 180.0f)); }
        static Quanterion Euler(float x, float y, float z) { return EulerRads(x * ((float)M_PI / 180.0f), y * ((float)M_PI / 180.0f), z * ((float)M_PI / 180.0f)); }
        static Quanterion EulerRads(Vector3 vec) { return EulerRads(vec.x, vec.y, vec.z); }
        // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
        static Quanterion EulerRads(float x, float y, float z) // YXZ
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
                c1 * c2 * s3 - s1 * s2 * c3,
                c1 * c2 * c3 + s1 * s2 * s3,
            };
        }
        
        static Quanterion Conjugate(const Quanterion& quat)
        {
            return { -quat.x, -quat.y, -quat.z, quat.w };
        }

        Vector3 eulerRads();
        Vector3 euler() { return eulerRads() * (180.0f / (float)M_PI); }

        Quanterion operator/(float other) const
        {
            return { x / other, y / other, z / other, w / other };
        }

        Quanterion operator*(const Quanterion& rhs)
        {
            return {
                this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y,
                this->w * rhs.y + this->y * rhs.w + this->z * rhs.x - this->x * rhs.z,
                this->w * rhs.z + this->z * rhs.w + this->x * rhs.y - this->y * rhs.x,
                this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z
            };
        }

        Quanterion operator*= (const Quanterion& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        Vector3 operator*(Vector3 point)
        {
            // Multiply by XYZ direction

            Quanterion q = *this * Quanterion(point.x, point.y, point.z, 0.0f) * Quanterion::Conjugate(*this);
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

        // void Rotate(const Vector3& vec)
        // {
        //     *this = Euler(vec) * *this;
        // }
	};
}