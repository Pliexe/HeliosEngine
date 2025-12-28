#pragma once

#include "pch.h"
#include "Vector.h"
#include "Matrix.h"

namespace Helios
{
    inline FVector3 operator*(const FVector3& vec, const Matrix4x4& mat) {
        return FVector3 {
            vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31,
            vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32,
            vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33
        };
    }

    inline FVector4 operator*(const FVector4& vec, const Matrix4x4& mat) {
        return FVector4 {
            vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31 + vec.w * mat._41,
            vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32 + vec.w * mat._42,
            vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33 + vec.w * mat._43,
            vec.x * mat._14 + vec.y * mat._24 + vec.z * mat._34 + vec.w * mat._44
        };
    }

}