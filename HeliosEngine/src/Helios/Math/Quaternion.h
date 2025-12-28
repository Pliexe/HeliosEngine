#pragma once

#include "pch.h"

#include "VectorImpl.h"

namespace Helios {
    struct Matrix4x4;
	struct Quaternion
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
        Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) { };

        const std::string to_string() const
        {
            return std::format("Quaternion (w: {}, x: {}i, y: {}j, z: {}k)", w, x, y, z); 
        }

        static float Dot(const Quaternion& lhs, const Quaternion& rhs)
        {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        // Operator check if Quaternion is equal using Dot product


        static const Quaternion Identity() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

        static Quaternion FromEuler(FVector3 vec);
        static Quaternion FromEuler(float x, float y, float z) { return FromEulerRads(x * ((float)H_PI / 180.0f), y * ((float)H_PI / 180.0f), z * ((float)H_PI / 180.0f)); }
        static Quaternion FromEulerRads(FVector3 vec);
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

        static Quaternion Normalize(Quaternion quat) // make copy
        {
            quat.normalize();
            return quat;
        }

        void normalize()
        {
            float len = std::sqrt(Dot(*this, *this));
            if (len > 0.0f)
            {
                float invLen = 1.0f / len;
                *this = { x * invLen, y * invLen, z * invLen, w * invLen };
            }
            else
            {
                *this = Quaternion::Identity();
            }
        }

        static Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
        {
            // Quaternion result;
            // result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
            // result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
            // result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
            // result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
            // return result;

            // I don't know if this implementation is faster, but it's less readable 
            return {
                lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y, // i
                lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x, // j
                lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w, // k
                lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z  // r
            };
		}

        static FVector3 ToEulerRads(const Quaternion& quaternion);
		static FVector3 ToEulerDeg(const Quaternion& quaternion);

        FVector3 eulerRads();
        FVector3 euler();

        Quaternion operator/(float other) const
        {
            return { x / other, y / other, z / other, w / other };
        }

        Quaternion operator*(const Quaternion& rhs)
        {
            return {
                this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y, // i
                this->w * rhs.y - this->x * rhs.z + this->y * rhs.w + this->z * rhs.x, // j
                this->w * rhs.z + this->x * rhs.y - this->y * rhs.x + this->z * rhs.w, // k
                this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z  // r
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

        FVector3 forward();
        FVector3 right();
        FVector3 up();

        Quaternion operator*(const Quaternion& rotation) const
        {
            return {
                w * rotation.x + x * rotation.w + y * rotation.z - z * rotation.y,
                w * rotation.y + y * rotation.w + z * rotation.x - x * rotation.z,
                w * rotation.z + z * rotation.w + x * rotation.y - y * rotation.x,
                w * rotation.w - x * rotation.x - y * rotation.y - z * rotation.z
            };
        }

        static Quaternion FromMatrix(const Matrix4x4& projection);

        static Quaternion FromAxisAngle(const FVector3& axis, float angle);

        // void Rotate(const Vector3& vec)
        // {
        //     *this = Euler(vec) * *this;
        // }
	};
}

#include "Quaternion.inl"