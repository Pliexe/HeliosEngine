/* Copyright (c) 2022 Szabadi László Zsolt
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

		operator float* () { return &x; }

		static Vector2 right()	{ return {  1.0f,  0.0f }; }
		static Vector2 left()	{ return { -1.0f,  0.0f }; }
		static Vector2 up()		{ return {  0.0f,  1.0f }; }
		static Vector2 down()	{ return {  0.0f, -1.0f }; }

		float length();
		Vector2 normalize();

		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;

		Vector2 operator+(Vector2& other);
		Vector2 operator-(Vector2& other);

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

		Vector2& operator=(Vector2& other);
	};

	struct HELIOS_API Vector3 : Vector2 {
		float z;

		Vector3() : Vector2(0.0f, 0.0f), z(0.0f) { };
		Vector3(float x, float y, float z) : Vector2(x, y), z(z) { };

		static Vector3 right()		{ return {  1.0f,  0.0f,  0.0f }; }
		static Vector3 left()		{ return { -1.0f,  0.0f,  0.0f }; }
		static Vector3 up()			{ return {  0.0f,  1.0f,  0.0f }; }
		static Vector3 down()		{ return {  0.0f, -1.0f,  0.0f }; }

		static Vector3 forward()	{ return {  0.0f,  0.0f,  1.0f }; }
		static Vector3 backwards()	{ return {  0.0f,  0.0f, -1.0f }; }

		float length();
		Vector3 normalize();

		Vector3 operator+(const Vector3& other) const;
		Vector3 operator-(const Vector3& other) const;
			  
		Vector3 operator+(Vector3& other);
		Vector3 operator-(Vector3& other);
			  
		Vector3 operator*(float n) const;
		Vector3 operator/(float n) const;
			  
		Vector3 operator*(float n);
		Vector3 operator/(float n);

		Vector3& operator+=(const Vector3& other);
		Vector3& operator-=(const Vector3& other);
			  
		Vector3& operator+=(Vector3& other);
		Vector3& operator-=(Vector3& other);
			  
		Vector3& operator*=(float n);
		Vector3& operator/=(float n);

		Vector3& operator=(Vector3& other);
	};

	struct HELIOS_API Point : public Vector2 {
	public:
		Point(float x, float y) : Vector2(x, y) { }
		Point() : Vector2() { }
		Point(const Point& size) : Vector2(size) { }
		Point(const Vector2& size) : Vector2(size) { }
	};

	class HELIOS_API Size2D : public Vector2
	{
	public:
		Size2D(float width, float height) : Vector2(width, height) { }
		Size2D() : Vector2() {}
		Size2D(const Size2D& size) : Vector2(size) { }
		Size2D(const Vector2& size) : Vector2(size) { }

		Vector2 normalize() = delete;
		float length() = delete;

		inline float width() const;
		inline float height() const;

		void setWidth(float width);
		void setHeight(float height);
		void setSize(float width, float height);
		void setSize(const Size2D size);

		friend class Transform;
	};
}