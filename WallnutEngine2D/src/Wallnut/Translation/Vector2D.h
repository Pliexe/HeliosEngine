/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class WALLNUT_API Vector2D
	{
	protected:
		float m_x, m_y;

	public:
		Vector2D();
		Vector2D(float x, float y);
		Vector2D(const Vector2D&);

		inline float getX() const;
		inline float getY() const;

		inline float x() const;
		inline float y() const;

		void setX(float x);
		void setY(float y);

		void setXY(float x, float y);
		void setXY(const Vector2D);

		Vector2D normalize();

		Vector2D operator+(const Vector2D& v) const;
		Vector2D operator-(const Vector2D& v) const;
		Vector2D operator+(Vector2D& v);
		Vector2D operator-(Vector2D& v);
		Vector2D operator*(float n);
		Vector2D operator/(float n);
		
		friend class Transform;
	};

	class WALLNUT_API Size2D : public Vector2D
	{
	public:
		Size2D(float width, float height) : Vector2D(width, height) { }
		Size2D() : Vector2D() {}
		Size2D(const Size2D& size) : Vector2D(size) { }
		Size2D(const Vector2D& size) : Vector2D(size) { }

		Vector2D normalize() = delete;

		inline float getWidth() const;
		inline float getHeight() const;

		inline float width() const;
		inline float height() const;

		void setWidth(float width);
		void setHeight(float height);
		void setSize(float width, float height);
		void setSize(const Size2D size);

		friend class Transform;
	};
}