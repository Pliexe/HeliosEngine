#pragma once
#include "Helios/Math/Vector.h"

namespace Helios
{
	enum class BodyType
	{
		Static,
		Dynamic,
		Kinematic,
	};

	enum class BodyShape
	{
		Circle,
		Box,
		Polygon,
	};

	struct HELIOS_API Rigidbody2D
	{
		BodyType type = BodyType::Dynamic;

		Vector2 velocity = Vector2::Zero();
		float mass = 1.0f;
		bool useGravity = true;
		bool isKinematic = false;
		Rigidbody2D() = default;
		Rigidbody2D(const Vector2& velocity, float mass, bool useGravity, bool isKinematic) : velocity(velocity), mass(mass), useGravity(useGravity), isKinematic(isKinematic) {}
	};

	struct CircleBody2D;
	struct HELIOS_API CircleCollider2D
	{
		Vector2 offset = Vector2::Zero();
		float radius = 1.0f;
		bool isTrigger = false;
		CircleBody2D* body = nullptr;

		CircleCollider2D() = default;
		CircleCollider2D(float radius, bool isTrigger) : radius(radius), isTrigger(isTrigger) {}
	};

	struct BoxBody2D;
	struct HELIOS_API BoxCollider2D
	{
		Vector2 offset = Vector2::Zero();
		Vector2 size = Vector2::One();
		bool isTrigger = false;

		BoxBody2D* body = nullptr;

		BoxCollider2D() = default;
		BoxCollider2D(const Vector2& size, bool isTrigger) : size(size), isTrigger(isTrigger) {}
	};
}
