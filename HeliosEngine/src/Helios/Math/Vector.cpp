/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#include "Vector.h"
#include "Matrix.h"

namespace Helios {

#pragma region Vector2

	Vector2 Vector2::normalize()
	{
		float length = this->length();
		if (length == 0.0f) return { 0.0f, 0.0f };
		else return { x / length, y / length };
	}

	Vector2 Vector2::operator*(const Vector2& vector2) const
	{
		Vector2 result;
		result.x = x * vector2.x;
		result.y = y * vector2.y;
		return result;
	}

	float Vector2::Dot(Vector2 lhv, Vector2 rhv)
	{
		return lhv.x * rhv.x + lhv.y * rhv.y;
	}

	float Vector2::Length(Vector2 a) { return sqrt(Dot(a, a)); }
	float Vector2::SqrLength(Vector2 a) { return Dot(a, a); }

	float Vector2::Distance(Vector2 a, Vector2 b)
	{
		return Length(a - b);
	}

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

#pragma endregion

#pragma region Vector3
	
	const Vector3 Vector3::right = { 1.0f, 0.0f, 0.0f };

	Vector3 Vector3::normalize()
	{
		float length = this->length();
		if (length == 0.0f) return { 0.0f, 0.0f, 0.0f };
		else return { x / length, y / length, z / length };
	}

    float Vector3::magnitude()
    {
		return sqrt(x * x + y * y + z * z);
    }

    float Vector3::Dot(Vector3 lhv, Vector3 rhv)
	{
		return lhv.x * rhv.x + lhv.y * rhv.y + lhv.z * rhv.z;
	}

	float Vector3::Length(Vector3 a) { return sqrt(Dot(a, a)); }
	float Vector3::SqrLength(Vector3 a) { return Dot(a, a); }

	float Vector3::Distance(Vector3 a, Vector3 b)
	{
		return Length(a - b);
	}

	float Vector3::DistanceSqrt(Vector3 a, Vector3 b)
	{
		return std::sqrt(SqrLength(a - b));
	}

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

	Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t)
	{
		return a + (b - a) * t;
	}

	Vector3 Vector3::MoveTowards(Vector3 a, Vector3 b, float maxDistanceDelta)
	{
		Vector3 delta = b - a;
		float sqrDelta = Vector3::SqrLength(delta);
		if (sqrDelta == 0.0f || (maxDistanceDelta >= 0.0f && sqrDelta <= maxDistanceDelta * maxDistanceDelta))
			return b;
		float deltaLength = sqrt(sqrDelta);
		return a + delta / deltaLength * maxDistanceDelta;
	}

	float Vector3::length() { return Length(*this); }
	float Vector3::sqrLength() { return SqrLength(*this); }

	Vector3 Vector3::operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 Vector3::operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 Vector3::operator+(Vector3& other) { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 Vector3::operator-(Vector3& other) { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 Vector3::operator-() { return { -x, -y, -z }; }

	Vector3 Vector3::operator+(Vector2& other) { return { x + other.x, y + other.y, z }; }
	Vector3 Vector3::operator-(Vector2& other) { return { x - other.x, y - other.y, z }; }

	Vector3 operator*(float n, const Vector3& other)
	{
		return { other.x * n, other.y * n, other.z * n };
	}

	Vector3 operator*(float n, Vector3& other)
	{
		return { other.x * n, other.y * n, other.z * n };
	}

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

	Vector3 Vector3::operator-=(Vector3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
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

	Vector3 Vector3::operator*(const Vector3& b) const
	{
		return { x * b.x, y * b.y, z * b.z };
	}

	Vector3 Vector3::operator/(const Vector3& scale) const
	{
		return { x / scale.x, y / scale.y, z / scale.z };
	}

	std::string Vector3::operator<<(const Vector3& other) const
	{
		return other.to_string();
	}

	std::string Vector3::operator<<(Vector3& other) const
	{
		return other.to_string();
	}

	std::string Vector3::to_string() const
	{
		return std::format("Vector3 (x: {}, y: {}, z: {})", x, y, z);
	}

	Vector3 Vector3::GetPosition(const Matrix4x4& projection)
	{
		return  { projection._14, projection._24, projection._34 };
	}

	float Vector3::AngleBetween(const Vector3& unit_vec1, const Vector3& unit_vec2)
	{
		float dot = Vector3::Dot(unit_vec1, unit_vec2);
		dot = std::clamp(dot, -1.0f, 1.0f);
		return acos(dot);
	}

	Vector3 Vector3::ProjectOnPlane(Vector3 pointOnPlane, Vector3 planeNormal, Vector3 point)
	{
		return point - Vector3::Project(point - pointOnPlane, planeNormal);
	}

	Vector3 Vector3::Rotate(const Vector3& normal, const Vector3& axis, float angle)
	{
		float s = sin(angle);
		float c = cos(angle);
		float oc = 1.0f - c;

		return
		{
			(normal.x * (axis.x * axis.x * oc + c) +
				normal.y * (axis.x * axis.y * oc - axis.z * s) +
				normal.z * (axis.x * axis.z * oc + axis.y * s)),

			(normal.x * (axis.y * axis.x * oc + axis.z * s) +
				normal.y * (axis.y * axis.y * oc + c) +
				normal.z * (axis.y * axis.z * oc - axis.x * s)),

			(normal.x * (axis.z * axis.x * oc - axis.y * s) +
				normal.y * (axis.z * axis.y * oc + axis.x * s) +
				normal.z * (axis.z * axis.z * oc + c))
		};
	}

	bool Vector3::operator==(const Vector3& other) const
	{
		return SqrLength(*this - other) < 9.99999944E-11f;
	}

	bool Vector3::operator==(Vector3& other) const
	{
		return SqrLength(*this - other) < 9.99999944E-11f;
	}

	/*Vector3 Vector3::operator=(Vector3& other)
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
	}*/

#pragma endregion

#pragma region Vector4

	float Vector4::Dot(Vector4 lhv, Vector4 rhv)
	{
		return lhv.x * rhv.x + lhv.y * rhv.y + lhv.z * rhv.z + lhv.w * rhv.w;
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

	Vector4 Vector4::operator*(const Matrix4x4& mat4x4) const
	{
		// vec4 * mat4x4
		return {
			x * mat4x4._11 + y * mat4x4._21 + z * mat4x4._31 + w * mat4x4._41,
			x * mat4x4._12 + y * mat4x4._22 + z * mat4x4._32 + w * mat4x4._42,
			x * mat4x4._13 + y * mat4x4._23 + z * mat4x4._33 + w * mat4x4._43,
			x * mat4x4._14 + y * mat4x4._24 + z * mat4x4._34 + w * mat4x4._44
		};
	}

	std::string Vector4::to_string() const
	{
		return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w);
	}

	Vector4 Vector4::operator=(Vector4& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

#pragma endregion


#pragma region Point
	
	Point Point::operator+(const Point& other) const
	{
		return { x + other.x, y + other.y };
	}

	Point Point::operator-(const Point& other) const
	{
		return { x - other.x, y - other.y };
	}

	Point Point::operator*(const Point& other) const
	{
		return { x * other.x, y * other.y };
	}

	Point Point::operator/(const Point& other) const
	{
		return { x / other.x, y / other.y };
	}

	Point Point::operator+=(const Point& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	Point Point::operator-=(const Point& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Point Point::operator*=(const Point& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		return *this;
	}

	Point Point::operator/=(const Point& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		return *this;
	}

	bool Point::operator==(const Point& other) const
	{
		return x == other.x && y == other.y;
	}
	
	bool Point::operator==(const Vector2& other) const
	{
		return x == other.x && y == other.y;
	}

#pragma endregion

#pragma region Size
	
	Size Size::operator+(const Size& other) const
	{
		return { width + other.width, height + other.height };
	}

	Size Size::operator-(const Size& other) const
	{
		return { width - other.width, height - other.height };
	}

	Size Size::operator*(const Size& other) const
	{
		return { width * other.width, height * other.height };
	}

	Size Size::operator/(const Size& other) const
	{
		return { width / other.width, height / other.height };
	}

	Size Size::operator+=(const Size& other)
	{
		this->width += other.width;
		this->height += other.height;
		return *this;
	}

	Size Size::operator-=(const Size& other)
	{
		this->width -= other.width;
		this->height -= other.height;
		return *this;
	}

	Size Size::operator*=(const Size& other)
	{
		this->width *= other.width;
		this->height *= other.height;
		return *this;
	}

	Size Size::operator/=(const Size& other)
	{
		this->width /= other.width;
		this->height /= other.height;
		return *this;
	}

	bool Size::operator==(const Size& other) const
	{
		return width == other.width && height == other.height;
	}
	
	bool Size::operator==(const Vector2& other) const
	{
		return width == other.x && height == other.y;
	}

#pragma endregion

}
