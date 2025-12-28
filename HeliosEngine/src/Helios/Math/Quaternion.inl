#pragma once

#include "Quaternion.h"
#include "Matrix.h"
#include "Vector.h"

namespace Helios
{
    inline FVector3 Quaternion::ToEulerRads(const Quaternion& quaternion)
    {
        Matrix4x4 matrix = Matrix4x4::RotationRow(quaternion);

        if (std::abs(matrix._32) < 0.9999999f) {
            return {
                asinf(-std::clamp(matrix._32, -1.0f, 1.0f)),
                atan2f(matrix._31, matrix._33),
                atan2f(matrix._12, matrix._22)
            };
        }
        else {
            return {
                asinf(-std::clamp(matrix._32, -1.0f, 1.0f)),
                atan2f(-matrix._13, matrix._11),
                0.0f
            };
        }
    }

    inline FVector3 Quaternion::eulerRads() //YXZ
    {
        Matrix4x4 matrix = Matrix4x4::RotationRow(*this);

        if (std::abs(matrix._32) < 0.9999999f) {
            return {
                asinf(-std::clamp(matrix._32, -1.0f, 1.0f)),
                atan2f(matrix._31, matrix._33),
                atan2f(matrix._12, matrix._22)
            };
        }
        else {
            return {
                asinf(-std::clamp(matrix._32, -1.0f, 1.0f)),
                atan2f(-matrix._13, matrix._11),
                0.0f
            };
        }
    }

    inline Quaternion Quaternion::FromMatrix(const Matrix4x4& projection)
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
    
    inline Quaternion Quaternion::FromEuler(FVector3 vec) { return FromEulerRads(vec * ((float)H_PI / 180.0f)); }
    inline Quaternion Quaternion::FromEulerRads(FVector3 vec) { return FromEulerRads(vec.x, vec.y, vec.z); }

    inline FVector3 Quaternion::ToEulerDeg(const Quaternion& quaternion) { return ToEulerDeg(quaternion) * (180.0f / static_cast<float>(H_PI)); }
    inline FVector3 Quaternion::euler() { return eulerRads() * (180.0f / (float)H_PI); }

    inline FVector3 operator*(const Quaternion& thisQ, const FVector3& point) {
        // Multiply by XYZ direction

        Quaternion q = thisQ * Quaternion(point.x, point.y, point.z, 0.0f) * Quaternion::Conjugate(thisQ);
        return { q.x, q.y, q.z };
    }

    inline FVector3 Quaternion::forward()
    {
        return Normalize(*this) * Vector3::Forward();
    }

    inline FVector3 Quaternion::right()
    {
        return Normalize(*this) * Vector3::Right();
    }

    inline FVector3 Quaternion::up()
    {
        return Normalize(*this) * Vector3::Up();
    }

    inline Quaternion Quaternion::FromAxisAngle(const FVector3& axis, float angle)
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
}

