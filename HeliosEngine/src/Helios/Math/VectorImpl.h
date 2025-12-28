/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"


namespace Helios
{
	template <typename T, std::size_t S, bool IsVector>
	struct TVector
	{
		static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

		T data[S];

		TVector() : data{} { };
		TVector(const TVector&) = default;
		TVector& operator=(const TVector&) = default;

		template<typename... Args>
		TVector(Args... args) : data{ static_cast<T>(args)... } {}

		operator T* () { return &data[0]; }
		operator const T* () const { return &data[0]; }

	};
}

#include "VectorOps.inl"

namespace Helios
{
	static constexpr bool IsVector = true;

	template <typename T>
	struct TVector<T, 2, IsVector> : TVectorOps<TVector<T, 2, IsVector>, 2, IsVector>
	{
		union {
			T data[2];
			struct { T x, y; };
		};

		TVector() : x(0), y(0) { };
		TVector(T x, T y) : x(x), y(y) { };
		TVector(const TVector<T, 3, IsVector>& other) : x(other.x), y(other.y) { };

		static TVector Zero() 	{ return {  0,  0 }; }
		static TVector One() 	{ return {  1,  1 }; }

		static TVector Right() 	{ return {  1,  0 }; }
		static TVector Left() 	{ return { -1,  0 }; }
		static TVector Up() 	{ return {  0,  1 }; }
		static TVector Down() 	{ return {  0, -1 }; }

		static inline float Angle(const TVector& lhs, const TVector& rhs) { return std::atan2(lhs.y, lhs.x) - std::atan2(rhs.y, rhs.x); }
	};

	struct Size : TVectorOps<TVector<uint32, 2, !IsVector>, 2, !IsVector>
	{
		union {
			uint32 data[2];
			struct { uint32 w, h; };
			struct { uint32 width, height; };
		};

		Size() : w(0), h(0) { };
		Size(uint32 x, uint32 y) : w(x), h(y) { };
	};

	struct Point : TVectorOps<TVector<int32, 2, !IsVector>, 2, !IsVector>
	{
		union {
			int32 data[2];
			struct { int32 x, y; };
		};

		Point() : x(0), y(0) { };
		Point(int32 x, int32 y) : x(x), y(y) { };
	};

	template <typename T>
	struct TVector<T, 3, IsVector> : TVectorOps<TVector<T, 3, IsVector>, 3, IsVector>
	{
		union {
			T data[3];
			struct { T x, y, z; };
		};

		TVector() : x(0), y(0), z(0) { };
		TVector(T x, T y, T z) : x(x), y(y), z(z) { };
		TVector(const TVector<T, 2, IsVector>& other) : x(other.x), y(other.y), z(0) { };
		TVector(const TVector<T, 4, IsVector>& other) : x(other.x), y(other.y), z(other.z) { };

		static TVector Zero() 	{ return {  0,  0,  0 }; }
		static TVector One() 	{ return {  1,  1,  1 }; }

		static TVector Forward() 	{ return {  0,  0,  1 }; }
		static TVector Back() 		{ return {  0,  0, -1 }; }
		static TVector Up() 		{ return {  0,  1,  0 }; }
		static TVector Down() 		{ return {  0, -1,  0 }; }
		static TVector Right() 		{ return {  1,  0,  0 }; }
		static TVector Left() 		{ return { -1,  0,  0 }; }

		static inline TVector Cross(const TVector& lhs, const TVector& rhs)
		{
			return { 
				lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.z * rhs.x - lhs.x * rhs.z,
				lhs.x * rhs.y - lhs.y * rhs.x
			};
		}

		static inline float Angle(const TVector& lhs, const TVector& rhs) 
		{
			return std::atan2(Cross(lhs, rhs).magnitude(), Dot(lhs, rhs));
		}
	};

	template <typename T>
	struct TVector<T, 4, IsVector> : TVectorOps<TVector<T, 4, IsVector>, 4, IsVector>
	{
		union {
			T data[4];
			struct { T x, y, z, w; };
		};

		TVector() : x(0), y(0), z(0), w(0) { };
		TVector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { };
		TVector(const TVector<T, 3, IsVector>& other) : x(other.x), y(other.y), z(other.z), w(0) { };
		TVector(const TVector<T, 2, IsVector>& other) : x(other.x), y(other.y), z(0), w(0) { };
	};

	template <typename T>
	using TVector2 = TVector<T, 2, IsVector>;

	template <typename T>
	using TVector3 = TVector<T, 3, IsVector>;

	template <typename T>
	using TVector4 = TVector<T, 4, IsVector>;

	using IVector2 = TVector2<int32_t>;
	using IVector3 = TVector3<int32_t>;
	using IVector4 = TVector4<int32_t>;

	using FVector2 = TVector2<float>;
	using FVector3 = TVector3<float>;
	using FVector4 = TVector4<float>;

	using Vector2 = FVector2;
	using Vector3 = FVector3;
	using Vector4 = FVector4;

	static_assert(sizeof(FVector2) == sizeof(float) * 2);
	static_assert(sizeof(FVector3) == sizeof(float) * 3);
	static_assert(sizeof(FVector4) == sizeof(float) * 4);
}