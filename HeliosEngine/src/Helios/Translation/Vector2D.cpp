/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Vector2D.h"

namespace Helios {

#pragma region Vector2

	float Vector2::length() { return sqrt(x * x + y * y); }
	Vector2 Vector2::normalize()
	{
		float length = this->length();
		return { x / length, y / length };
	}

	Vector2 Vector2::operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
	Vector2 Vector2::operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }
	Vector2 Vector2::operator+(Vector2& other) { return { x + other.x, y + other.y }; }
	Vector2 Vector2::operator-(Vector2& other) { return { x - other.x, y - other.y }; }

	Vector2 Vector2::operator*(float n) const { return { x * n, y * n }; }
	Vector2 Vector2::operator/(float n) const { return { x / n, y / n }; }
	Vector2 Vector2::operator*(float n) { return { x * n, y * n }; }
	Vector2 Vector2::operator/(float n) { return { x / n, y / n }; }

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

	Vector2& Vector2::operator=(Vector2& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

#pragma endregion

#pragma region Vector3
	
	float Vector3::length() { return sqrt(x * x + y * y + z * z); }
	Vector3 Vector3::normalize()
	{
		float length = this->length();
		return { x / length, y / length, z / length };
	}

	Vector3 Vector3::operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 Vector3::operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 Vector3::operator+(Vector3& other) { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 Vector3::operator-(Vector3& other) { return { x - other.x, y - other.y, z - other.z }; }
		  		  
	Vector3 Vector3::operator*(float n) const { return { x * n, y * n, z * n }; }
	Vector3 Vector3::operator/(float n) const { return { x / n, y / n, z / n }; }
	Vector3 Vector3::operator*(float n) { return { x * n, y * n, z * n }; }
	Vector3 Vector3::operator/(float n) { return { x / n, y / n, z / n }; }
	
	Vector3& Vector3::operator+=(const Vector3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}
	Vector3& Vector3::operator-=(const Vector3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}
	Vector3& Vector3::operator*=(float n)
	{
		this->x *= n;
		this->y *= n;
		this->z *= n;
		return *this;
	}
	Vector3& Vector3::operator/=(float n)
	{
		this->x /= n;
		this->y /= n;
		this->z /= n;
		return *this;
	}

	Vector3& Vector3::operator=(Vector3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}
	

#pragma endregion

#pragma region Size2D
	
	float Size2D::width()	const { return x; }
	float Size2D::height()	const { return y; }

	void Size2D::setWidth(float width) { x = width; }
	void Size2D::setHeight(float height) { y = height; }
	void Size2D::setSize(float width, float height) { x = width; y = height; }

	void Size2D::setSize(const Size2D size)
	{
		x = size.x;
		y = size.y;
	}

#pragma endregion

}
