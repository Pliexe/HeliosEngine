/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Vector.h"

namespace Helios {

#pragma region Vector2

	Vector2 Vector2::normalize()
	{
		float length = this->length();
		if (length == 0.0f) return { 0.0f, 0.0f };
		else return { x / length, y / length };
	}

	float Vector2::Dot(Vector2 lhv, Vector2 rhv)
	{
		return { lhv.x * rhv.x + lhv.y * rhv.y };
	}

	float Vector2::Length(Vector2 a) { return sqrt(Dot(a, a)); }
	float Vector2::SqrLength(Vector2 a) { return Dot(a, a); }
	float distance(Vector2 a, Vector2 b) { return Vector2::Length(a - b); }
	Vector2 Vector2::Project(Vector2 a, Vector2 n) { return n * (Vector2::Dot(a, n) / Vector2::Dot(n, n)); }

	float Vector2::length() { return Length(*this); }
	float Vector2::sqrLength() { return Length(*this); }

	Vector2 Vector2::operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
	Vector2 Vector2::operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
	Vector2 Vector2::operator+(Vector2& other) { return { x + other.x, y + other.y }; }
	Vector2 Vector2::operator-(Vector2& other) { return { x - other.x, y - other.y }; }
	Vector2 Vector2::operator-() { return { -x, -y }; }

	Vector2 Vector2::operator*(float n) const { return { x * n, y * n }; }
	Vector2 Vector2::operator/(float n) const { return { x / n, y / n }; }
	Vector2 Vector2::operator*(float n) { return { x * n, y * n }; }
	Vector2 Vector2::operator/(float n) { return { x / n, y / n }; }

	bool Vector2::operator==(const Vector2& other) const { return SqrLength(*this - other) < 9.99999944E-11f; }
	bool Vector2::operator==(Vector2& other) const { return SqrLength(*this - other) < 9.99999944E-11f; }

	Vector2& Vector2::operator+=(const Vector2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	Vector2& Vector2::operator-=(const Vector2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Vector2& Vector2::operator*=(float n)
	{
		this->x *= n;
		this->y *= n;
		return *this;
	}
	Vector2& Vector2::operator/=(float n)
	{
		this->x /= n;
		this->y /= n;
		return *this;
	}

	Vector2& Vector2::operator=(Vector2 other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	Vector2& Vector2::operator=(Vector2& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

#pragma endregion

#pragma region Vector3
	
	const Vector3 Vector3::right = { 1.0f, 0.0f, 0.0f };

	Vector3 Vector3::normalize()
	{
		float length = this->length();
		if (length == 0.0f) return { 0.0f, 0.0f, 0.0f };
		else return { x / length, y / length, z / length };
	}

	float Vector3::Dot(Vector3 lhv, Vector3 rhv)
	{
		return { lhv.x * rhv.x + lhv.y * rhv.y + lhv.z * rhv.z };
	}

	float Vector3::Length(Vector3 a) { return sqrt(Dot(a, a)); }
	float Vector3::SqrLength(Vector3 a) { return Dot(a, a); }
	float distance(Vector3 a, Vector3 b) { return Vector3::Length(a - b); }
	Vector3 Vector3::Project(Vector3 a, Vector3 n) { return n * (Vector3::Dot(a, n) / Vector3::Dot(n, n)); }
	Vector3	Vector3::Cross(Vector3 a, Vector3 b)
	{
		// Cx = Ay * Bz - Az * By
		// Multiply with other (The cross products (The name))
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	float Vector3::length() { return Length(*this); }
	float Vector3::sqrLength() { return Length(*this); }

	Vector3 Vector3::operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 Vector3::operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 Vector3::operator+(Vector3& other) { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 Vector3::operator-(Vector3& other) { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 Vector3::operator-() { return { -x, -y, -z }; }

	Vector3 Vector3::operator+(Vector2& other) { return { x + other.x, y + other.y, z }; }
	Vector3 Vector3::operator-(Vector2& other) { return { x - other.x, y - other.y, z }; }
		  		  
	Vector3 Vector3::operator*(float n) const { return { x * n, y * n, z * n }; }
	Vector3 Vector3::operator/(float n) const { return { x / n, y / n, z / n }; }
	Vector3 Vector3::operator*(float n) { return { x * n, y * n, z * n }; }
	Vector3 Vector3::operator/(float n) { return { x / n, y / n, z / n }; }
	
	Vector3 Vector3::operator+=(const Vector3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}
	Vector3 Vector3::operator+=(Vector3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}
	Vector3 Vector3::operator-=(const Vector3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}
	Vector3 Vector3::operator*=(float n)
	{
		this->x *= n;
		this->y *= n;
		this->z *= n;
		return *this;
	}
	Vector3 Vector3::operator/=(float n)
	{
		this->x /= n;
		this->y /= n;
		this->z /= n;
		return *this;
	}

	bool Vector3::operator==(const Vector3& other) const
	{
		return SqrLength(*this - other) < 9.99999944E-11f;
	}

	bool Vector3::operator==(Vector3& other) const
	{
		return SqrLength(*this - other) < 9.99999944E-11f;
	}

	Vector3 Vector3::operator=(Vector3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	Vector3 Vector3::operator=(Vector3 other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

#pragma endregion

#pragma region Vector4

	float Vector4::Dot(Vector4 lhv, Vector4 rhv)
	{
		return { lhv.x * rhv.x + lhv.y * rhv.y + lhv.z * rhv.z + lhv.w * rhv.w };
	}

	Vector4 Vector4::normalize()
	{
		float length = this->length();
		if (length == 0.0f) return Vector4::Zero();
		else return { x / length, y / length, z / length, w / length };
	}

	float Vector4::Length(Vector4 a) { return sqrt(Dot(a, a)); }
	float Vector4::SqrLength(Vector4 a) { return Dot(a, a); }
	float distance(Vector4 a, Vector4 b) { return Vector4::Length(a - b); }
	Vector4 Vector4::Project(Vector4 a, Vector4 n) { return n * (Vector4::Dot(a,n) / Vector4::Dot(n,n)); }

	float Vector4::length() { return Length(*this); }
	float Vector4::sqrLength() { return Length(*this); }

	Vector4 Vector4::operator+(const Vector4& other) const { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
	Vector4 Vector4::operator-(const Vector4& other) const { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
	Vector4 Vector4::operator+(Vector4& other) { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
	Vector4 Vector4::operator-(Vector4& other) { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
	Vector4 Vector4::operator+(Vector3& other) { return { x + other.x, y + other.y, z + other.z, w }; }
	Vector4 Vector4::operator-(Vector3& other) { return { x - other.x, y - other.y, z - other.z, w }; }
	Vector4 Vector4::operator+(Vector2& other) { return { x + other.x, y + other.y, z, w }; }
	Vector4 Vector4::operator-(Vector2& other) { return { x - other.x, y - other.y, z, w }; }
	Vector4 Vector4::operator-() { return { -x, -y, -z, -w }; }
		  		  
	Vector4 Vector4::operator*(float n) const { return { x * n, y * n, z * n, w * n }; }
	Vector4 Vector4::operator/(float n) const { return { x / n, y / n, z / n, w / n }; }
	Vector4 Vector4::operator*(float n) { return { x * n, y * n, z * n, w * n }; }
	Vector4 Vector4::operator/(float n) { return { x / n, y / n, z / n, w / n }; }

	bool Vector4::operator==(const Vector4& other) const { return SqrLength(*this - other) < 9.99999944E-11f; }
	bool Vector4::operator==(Vector4& other) const { return SqrLength(*this - other) < 9.99999944E-11f; }

	Vector4 Vector4::operator+=(const Vector4& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
		return *this;
	}
	Vector4 Vector4::operator-=(const Vector4& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
		return *this;
	}

	Vector4 Vector4::operator*=(float n)
	{
		this->x *= n;
		this->y *= n;
		this->z *= n;
		this->w *= n;
		return *this;
	}
	Vector4 Vector4::operator/=(float n)
	{
		this->x /= n;
		this->y /= n;
		this->z /= n;
		this->w /= n;
		return *this;
	}

	Vector4 Vector4::operator=(Vector4 other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->w = other.w;
		return *this;
	}

	Vector4 Vector4::operator=(Vector4& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

#pragma endregion


#pragma region Size
	
	float Size::width()	const { return x; }
	float Size::height()	const { return y; }

	void Size::setWidth(float width) { x = width; }
	void Size::setHeight(float height) { y = height; }
	void Size::setSize(float width, float height) { x = width; y = height; }

	void Size::setSize(const Size size)
	{
		x = size.x;
		y = size.y;
	}

#pragma endregion

}
