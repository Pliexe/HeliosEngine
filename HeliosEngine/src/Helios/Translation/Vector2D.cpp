/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Vector2D.h"

namespace Helios {

#pragma region Constructors

	Vector2D::Vector2D()
	{
		vec2[0] = 0;
		vec2[1] = 0;
	}

	Vector2D::Vector2D(float x, float y)
	{
		this->vec2[0] = x;
		this->vec2[1] = y;
	}

	Vector2D::Vector2D(const Vector2D& other)
	{
		this->vec2[0] = other.vec2[0];
		this->vec2[1] = other.vec2[1];
	}

#pragma endregion

#pragma region Getter, Setters

	float Vector2D::getX() const { return vec2[0]; }
	float Vector2D::getY() const { return vec2[1]; }

	float Vector2D::x() const { return vec2[0]; }
	float Vector2D::y() const { return vec2[1]; }

	void Vector2D::setX(float _x) { vec2[0] = _x; }
	void Vector2D::setY(float _y) { vec2[1] = _y; }

	void Vector2D::setXY(float x, float y)
	{
		this->vec2[0] = x;
		this->vec2[1] = y;
	}

	void Vector2D::setXY(const Vector2D other)
	{
		this->vec2[0] = other.vec2[0];
		this->vec2[1] = other.vec2[1];
	}

#pragma endregion

#pragma region Methods

	Vector2D Vector2D::normalize() {
		Vector2D result;
		float length = sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);
		return Vector2D(vec2[0] / length, vec2[1] / length);
	}

#pragma endregion

#pragma region Addition operators


	Vector2D Vector2D::operator+(const Vector2D& other) const
	{
		return Vector2D(vec2[0] + other.vec2[0], vec2[1] + other.vec2[1]);
	}

	Vector2D Vector2D::operator-(const Vector2D& other) const
	{
		return Vector2D(vec2[0] - other.vec2[0], vec2[1] - other.vec2[1]);
	}

	Vector2D Vector2D::operator+(Vector2D& other)
	{
		return Vector2D(vec2[0] + other.vec2[0], vec2[1] + other.vec2[1]);
	}

	Vector2D Vector2D::operator-(Vector2D& other)
	{
		return Vector2D(vec2[0] - other.vec2[0], vec2[1] - other.vec2[1]);
	}

	Vector2D Vector2D::operator*(float n) {
		return Vector2D(vec2[0] * n, vec2[1] * n);
	}

	Vector2D Vector2D::operator/(float n) {
		return Vector2D(vec2[0] / n, vec2[1] / n);
	}

#pragma endregion

#pragma region Assigment Operators

	Vector2D Vector2D::operator+=(const Vector2D& other)
	{
		vec2[0] += other.vec2[0];
		vec2[1] += other.vec2[1];
		return *this;
	}

	Vector2D Vector2D::operator-=(const Vector2D& other)
	{
		vec2[0] -= other.vec2[0];
		vec2[1] -= other.vec2[1];
		return *this;
	}

	Vector2D Vector2D::operator+=(Vector2D& other)
	{
		vec2[0] += other.vec2[0];
		vec2[1] += other.vec2[1];
		return *this;
	}

	Vector2D Vector2D::operator-=(Vector2D& other)
	{
		vec2[0] -= other.vec2[0];
		vec2[1] -= other.vec2[1];
		return *this;
	}

	Vector2D Vector2D::operator*=(float n) {
		vec2[0] *= n;
		vec2[1] *= n;
		return *this;
	}

	Vector2D Vector2D::operator/=(float n) {
		vec2[0] /= n;
		vec2[1] /= n;
		return *this;
	}

#pragma endregion

#pragma region Size2D

	float Size2D::getWidth() const { return vec2[0]; }
	float Size2D::getHeight() const { return vec2[1]; }

	float Size2D::width() const { return vec2[0]; }
	float Size2D::height() const { return vec2[1]; }

	void Size2D::setWidth(float width) { vec2[0] = width; }
	void Size2D::setHeight(float height) { vec2[1] = height; }
	void Size2D::setSize(float width, float height) { vec2[0] = width; vec2[1] = height; }

	void Size2D::setSize(const Size2D size)
	{
		vec2[0] = size.vec2[0];
		vec2[1] = size.vec2[1];
	}

#pragma endregion

}
