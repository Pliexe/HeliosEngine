/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios {

	struct HELIOS_API Vector2 {
		float x;
		float y;

		Vector2() : x(0.0f), y(0.0f) { };
		Vector2(float x, float y) : x(x), y(y) { };
		Vector2(const Vector2& other) : x(other.x), y(other.y) { };

		operator float* () { return &x; }

		static inline Vector2 Right()	{ return {  1.0f,  0.0f }; }
		static inline Vector2 Left()	{ return { -1.0f,  0.0f }; }
		static inline Vector2 Up()		{ return {  0.0f,  1.0f }; }
		static inline Vector2 Down()	{ return {  0.0f, -1.0f }; }
		
		static inline Vector2 Zero()	{ return {  0.0f,  0.0f }; }
		static inline Vector2 One()		{ return {  1.0f,  1.0f }; }

		static inline float Dot(Vector2 lhv, Vector2 rhv);
		static inline float Length(Vector2 a);
		static inline float SqrLength(Vector2 a);
		static inline float Distance(Vector2 a, Vector2 b);
		static inline Vector2 Project(Vector2 a, Vector2 n);

		inline float length();
		inline float sqrLength();
		inline Vector2 normalize();

		bool operator==(const Vector2& other) const;
		bool operator==(Vector2& other) const;

		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;

		Vector2 operator+(Vector2& other);
		Vector2 operator-(Vector2& other);
		
		Vector2 operator-();

		Vector2 operator*(float n) const;
		Vector2 operator/(float n) const;

		Vector2 operator*(float n);
		Vector2 operator/(float n);

		Vector2& operator+=(const Vector2& other);
		Vector2& operator-=(const Vector2& other);
		
		Vector2& operator+=(Vector2& other);
		Vector2& operator-=(Vector2& other);
		
		Vector2& operator*=(float n);
		Vector2& operator/=(float n);

		Vector2& operator=(Vector2 other);
		Vector2& operator=(Vector2& other);
	};

	struct HELIOS_API Vector3Copy {
		union
		{
			float xyz[3];
			struct
			{
				float x;
				float y;
				float z;
			};
		};

		Vector3Copy() : x(0.0f), y(0.0f), z(0.0f) { };
		Vector3Copy(float x, float y, float z) : x(x), y(y), z(z) { };

		static inline Vector3Copy Right() { return { 1.0f,  0.0f,  0.0f }; }
		static inline Vector3Copy Left() { return { -1.0f,  0.0f,  0.0f }; }
		static inline Vector3Copy Up() { return { 0.0f,  1.0f,  0.0f }; }
		static inline Vector3Copy Down() { return { 0.0f, -1.0f,  0.0f }; }

		static inline Vector3Copy Forward() { return { 0.0f,  0.0f,  1.0f }; }
		static inline Vector3Copy Backwards() { return { 0.0f,  0.0f, -1.0f }; }

		static inline Vector3Copy Zero() { return { 0.0f,  0.0f,  0.0f }; }
		static inline Vector3Copy One() { return { 1.0f,  1.0f,  1.0f }; }

		static const Vector3Copy right;

		//Vector3Copy raaa = Vector3Copy { 1.0f,  0.0f,  0.0f };
		/*static inline const Vector3Copy Left { -1.0f,  0.0f,  0.0f }; }
		static inline const Vector3Copy Up { 0.0f,  1.0f,  0.0f }; }
		static inline const Vector3Copy Down { 0.0f, -1.0f,  0.0f }; }

		static inline const Vector3Copy Forward { 0.0f,  0.0f,  1.0f }; }
		static inline const Vector3Copy Backwards { 0.0f,  0.0f, -1.0f }; }

		static lVector3Copy Zero { 0.0f,  0.0f,  0.0f }; }*/
		//static inline const Vector3Copy One { 1.0f,  1.0f,  1.0f }; }

		static inline float Dot(Vector3Copy lhv, Vector3Copy rhv);
		static inline float Length(Vector3Copy a);
		static inline float SqrLength(Vector3Copy a);
		static inline float Distance(Vector3Copy a, Vector3Copy b);
		static inline Vector3Copy Project(Vector3Copy a, Vector3Copy n);
		static inline Vector3Copy Cross(Vector3Copy a, Vector3Copy b);

		inline float length();
		inline float sqrLength();
		inline Vector3Copy normalize();

		bool operator==(const Vector3Copy& other) const;
		bool operator==(Vector3Copy& other) const;

		Vector3Copy operator+(const Vector3Copy& other) const;
		Vector3Copy operator-(const Vector3Copy& other) const;

		Vector3Copy operator+(Vector3Copy& other);
		Vector3Copy operator-(Vector3Copy& other);
		Vector3Copy operator-();

		Vector3Copy operator+(Vector2& other);
		Vector3Copy operator-(Vector2& other);

		Vector3Copy operator*(float n) const;
		Vector3Copy operator/(float n) const;

		Vector3Copy operator*(float n);
		Vector3Copy operator/(float n);

		Vector3Copy operator+=(const Vector3Copy& other);
		Vector3Copy operator-=(const Vector3Copy& other);

		Vector3Copy operator+=(Vector3Copy& other);
		Vector3Copy operator-=(Vector3Copy& other);

		Vector3Copy operator*=(float n);
		Vector3Copy operator/=(float n);

		/*Vector3 operator=(Vector3& other);
		Vector3 operator=(Vector3 other);*/
	};

	struct HELIOS_API Vector3 : public Vector2 {
		float z;

		Vector3() : Vector2(0.0f, 0.0f), z(0.0f) { };
		Vector3(float x, float y, float z) : Vector2(x, y), z(z) { };
		Vector3(const Vector2& other) : Vector2(other), z(0.0f) { };
		Vector3(const Vector3& other) : Vector2(other.x, other.y), z(other.z) { };

		static inline Vector3 Right()		{ return {  1.0f,  0.0f,  0.0f }; }
		static inline Vector3 Left()		{ return { -1.0f,  0.0f,  0.0f }; }
		static inline Vector3 Up()			{ return {  0.0f,  1.0f,  0.0f }; }
		static inline Vector3 Down()		{ return {  0.0f, -1.0f,  0.0f }; }

		static inline Vector3 Forward()		{ return {  0.0f,  0.0f,  1.0f }; }
		static inline Vector3 Backwards()	{ return {  0.0f,  0.0f, -1.0f }; }

		static inline Vector3 Zero()		{ return {  0.0f,  0.0f,  0.0f }; }
		static inline Vector3 One()			{ return {  1.0f,  1.0f,  1.0f }; }

		static const Vector3 right;

		//Vector3 raaa = Vector3 { 1.0f,  0.0f,  0.0f };
		/*static inline const Vector3 Left { -1.0f,  0.0f,  0.0f }; }
		static inline const Vector3 Up { 0.0f,  1.0f,  0.0f }; }
		static inline const Vector3 Down { 0.0f, -1.0f,  0.0f }; }
					  
		static inline const Vector3 Forward { 0.0f,  0.0f,  1.0f }; }
		static inline const Vector3 Backwards { 0.0f,  0.0f, -1.0f }; }
					  
		static lVector3 Zero { 0.0f,  0.0f,  0.0f }; }*/
		//static inline const Vector3 One { 1.0f,  1.0f,  1.0f }; }

		static inline float Dot(Vector3 lhv, Vector3 rhv);
		static inline float Length(Vector3 a);
		static inline float SqrLength(Vector3 a);
		static inline float Distance(Vector3 a, Vector3 b);
		static inline Vector3 Project(Vector3 a, Vector3 n);
		static inline Vector3 Cross(Vector3 a, Vector3 b);

		inline float length();
		inline float sqrLength();
		inline Vector3 normalize();

		bool operator==(const Vector3& other) const;
		bool operator==(Vector3& other) const;

		Vector3 operator+(const Vector3& other) const;
		Vector3 operator-(const Vector3& other) const;
			  
		Vector3 operator+(Vector3& other);
		Vector3 operator-(Vector3& other);
		Vector3 operator-();
		
		Vector3 operator+(Vector2& other);
		Vector3 operator-(Vector2& other);
			  
		Vector3 operator*(float n) const;
		Vector3 operator/(float n) const;

		Vector3 operator*(float n);
		Vector3 operator/(float n);

		Vector3 operator+=(const Vector3& other);
		Vector3 operator-=(const Vector3& other);
			  
		Vector3 operator+=(Vector3& other);
		Vector3 operator-=(Vector3& other);
			  
		Vector3 operator*=(float n);
		Vector3 operator/=(float n);

		/*Vector3 operator=(Vector3& other);
		Vector3 operator=(Vector3 other);*/
	};

	struct HELIOS_API Vector4 : public Vector3 {
		float w;
		
		Vector4() : Vector3(0.0f, 0.0f, 0.0f), w(0.0f) { };
		Vector4(float x, float y, float z, float w) : Vector3(x, y, z), w(w) { };
		Vector4(const Vector2& other) : Vector3(other), w(0.0f) { };
		Vector4(const Vector3& other) : Vector3(other), w(0.0f) { };
		Vector4(const Vector4& other) : Vector3(other.x, other.y, other.z), w(other.w) { };
		
		static inline Vector4 Zero() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
		static inline Vector4 One()  { return { 1.0f, 1.0f, 1.0f, 1.0f }; }

		static inline float Dot(Vector4 lhv, Vector4 rhv);
		static inline float Length(Vector4 a);
		static inline float SqrLength(Vector4 a);
		static inline float Distance(Vector4 a, Vector4 b);
		static inline Vector4 Project(Vector4 a, Vector4 n);

		inline float length();
		inline float sqrLength();
		inline Vector4 normalize();

		bool operator==(const Vector4& other) const;
		bool operator==(Vector4& other) const;

		Vector4 operator+(const Vector4& other) const;
		Vector4 operator-(const Vector4& other) const;

		Vector4 operator+(Vector4& other);
		Vector4 operator-(Vector4& other);
		Vector4 operator-();

		Vector4 operator+(Vector3& other);
		Vector4 operator-(Vector3& other);

		Vector4 operator+(Vector2& other);
		Vector4 operator-(Vector2& other);

		Vector4 operator*(float n) const;
		Vector4 operator/(float n) const;

		Vector4 operator*(float n);
		Vector4 operator/(float n);

		Vector4 operator+=(const Vector4& other);
		Vector4 operator-=(const Vector4& other);

		Vector4 operator+=(Vector4& other);
		Vector4 operator-=(Vector4& other);

		Vector4 operator*=(float n);
		Vector4 operator/=(float n);

		Vector4 operator=(Vector4& other);
		Vector4 operator=(Vector4 other);
	};

	struct HELIOS_API Point : public Vector2 {
	public:
		Point(float x, float y) : Vector2(x, y) { }
		Point() : Vector2() { }
		Point(const Point& size) : Vector2(size) { }
		Point(const Vector2& size) : Vector2(size) { }
	};

	class HELIOS_API Size : public Vector2
	{
	public:
		Size(float width, float height) : Vector2(width, height) { }
		Size() : Vector2() {}
		Size(const Size& size) : Vector2(size) { }
		Size(const Vector2& size) : Vector2(size) { }

		Vector2 normalize() = delete;
		float length() = delete;

		inline float width() const;
		inline float height() const;

		void setWidth(float width);
		void setHeight(float height);
		void setSize(float width, float height);
		void setSize(const Size size);

		friend class Transform;
	};
}