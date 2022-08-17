/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Vector2D.h"

namespace Wallnut {

#pragma region Constructors

	Vector2D::Vector2D()
	{
		m_x = 0;
		m_y = 0;
	}

	Vector2D::Vector2D(float x, float y)
	{
		this->m_x = x;
		this->m_y = y;
	}

	Vector2D::Vector2D(const Vector2D& other)
	{
		this->m_x = other.m_x;
		this->m_y = other.m_y;
	}

#pragma endregion

#pragma region Getter, Setters

	float Vector2D::getX() const { return m_x; }
	float Vector2D::getY() const { return m_y; }

	float Vector2D::x() const { return m_x; }
	float Vector2D::y() const { return m_y; }

	void Vector2D::setX(float _x) { m_x = _x; }
	void Vector2D::setY(float _y) { m_y = _y; }

	void Vector2D::setXY(float x, float y)
	{
		this->m_x = x;
		this->m_y = y;
	}

	void Vector2D::setXY(const Vector2D other)
	{
		this->m_x = other.m_x;
		this->m_y = other.m_y;
	}

#pragma endregion

#pragma region Methods

	Vector2D Vector2D::normalize() {
		Vector2D result;
		float length = sqrt(m_x * m_x + m_y * m_y);
		return Vector2D(m_x / length, m_y / length);
	}

#pragma endregion

#pragma region Addition operators


	Vector2D Vector2D::operator+(const Vector2D& other) const
	{
		return Vector2D(m_x + other.m_x, m_y + other.m_y);
	}

	Vector2D Vector2D::operator-(const Vector2D& other) const
	{
		return Vector2D(m_x - other.m_x, m_y - other.m_y);
	}

	Vector2D Vector2D::operator+(Vector2D& other)
	{
		return Vector2D(m_x + other.m_x, m_y + other.m_y);
	}

	Vector2D Vector2D::operator-(Vector2D& other)
	{
		return Vector2D(m_x - other.m_x, m_y - other.m_y);
	}

	Vector2D Vector2D::operator*(float n) {
		return Vector2D(m_x * n, m_y * n);
	}

	Vector2D Vector2D::operator/(float n) {
		return Vector2D(m_x / n, m_y / n);
	}

#pragma endregion

#pragma region Assigment Operators

	Vector2D Vector2D::operator+=(const Vector2D& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		return *this;
	}

	Vector2D Vector2D::operator-=(const Vector2D& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		return *this;
	}

	Vector2D Vector2D::operator+=(Vector2D& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		return *this;
	}

	Vector2D Vector2D::operator-=(Vector2D& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		return *this;
	}

	Vector2D Vector2D::operator*=(float n) {
		m_x *= n;
		m_y *= n;
		return *this;
	}

	Vector2D Vector2D::operator/=(float n) {
		m_x /= n;
		m_y /= n;
		return *this;
	}

#pragma endregion

#pragma region Size2D

	float Size2D::getWidth() const { return m_x; }
	float Size2D::getHeight() const { return m_y; }

	float Size2D::width() const { return m_x; }
	float Size2D::height() const { return m_y; }

	void Size2D::setWidth(float width) { m_x = width; }
	void Size2D::setHeight(float height) { m_y = height; }
	void Size2D::setSize(float width, float height) { m_x = width; m_y = height; }

	void Size2D::setSize(const Size2D size)
	{
		m_x = size.m_x;
		m_y = size.m_y;
	}

#pragma endregion

}
