#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Core/Time.h"
#include "Helios/Scene/Components.h"
#include "Helios/Scene/Entity.h"

namespace Helios
{
	struct Body2DInformation
	{
		Vector2 position;
		float rotation;
		Vector2 velocity;
	};

	class HELIOS_API IBody2D
	{
	public:
		virtual void SetPosition(const Vector2& position) = 0;
		//virtual void SetVelocity(const Vector2& velocity) = 0;
		virtual void SetOffset(const Vector2& offset) = 0;
		virtual void SetRotation(float rotation) = 0;

		virtual Vector2 GetPosition() = 0;
		virtual Vector2 GetVelocity() = 0;
		virtual float GetRotation() = 0;
	};

	class HELIOS_API ICircleBody2D 
	{
	public:
		virtual void SetRadius(float radius) = 0;
	};

	class HELIOS_API IBoxBody2D
	{
	public:
		virtual void SetSize(const Vector2& size) = 0;
	};

	class HELIOS_API CircleBody2D : public IBody2D, public ICircleBody2D {};
	class HELIOS_API BoxBody2D : public IBody2D, public IBoxBody2D {};

	class HELIOS_API IPhysics2D
	{
	public:
		virtual CircleBody2D* CreateCircle(Entity entity, CircleCollider2D& collider) = 0;
		virtual BoxBody2D* CreateBox(Entity entity, BoxCollider2D& collider) = 0;
		
		virtual void OnStep() = 0;
	};

	class HELIOS_API Physics2D
	{
	private:
		inline static std::vector<std::tuple<Entity, TransformComponent, Rigidbody2D, CircleCollider2D>> m_CircleColliders;
		inline static std::vector<std::tuple<Entity, TransformComponent, Rigidbody2D, BoxCollider2D>> m_BoxColliders;
		inline static std::vector<std::tuple<TransformComponent, CircleCollider2D>> m_StaticCircleColliders;
		inline static std::vector<std::tuple<TransformComponent, BoxCollider2D>> m_StaticBoxColliders;

		static void Clear();

		inline static std::vector<Body2DInformation> m_Bodies;

		inline static enum class ImplementationType
		{
			HeliosPhysics,
			Box2D,
		} s_implementationType = ImplementationType::Box2D;

	public:

		static IPhysics2D& GetInstance();

		static CircleBody2D* CreateCircle(Entity entity, CircleCollider2D& collider) { return GetInstance().CreateCircle(entity, collider); }
		static BoxBody2D* CreateBox(Entity entity, BoxCollider2D& collider) { return GetInstance().CreateBox(entity, collider); }
		static void OnStep() { GetInstance().OnStep(); }

		/*static Body2D& UnregisterBody(Body2D& body)
		{
			auto it = std::find(m_Bodies.begin(), m_Bodies.end(), [&body](const Body2D& b) { return &b == &body; });

			if (it != m_Bodies.end()) {
				m_Bodies.erase(it);
			}
		}*/

		/*static Body2D& RegisterBody(Body2D& body)
		{
			m_Bodies.push_back(body);
		}*/

		static void Init()
		{
			m_Bodies.reserve(100);
		}

		static void Shutdown()
		{
			m_Bodies.clear();
		}

		//static void Init();
		//static void Shutdown();

		static void Submit(Entity entity, TransformComponent& transform, Rigidbody2D& rigidbody, CircleCollider2D& collider);
		static void Submit(Entity entity, TransformComponent& transform, Rigidbody2D& rigidbody, BoxCollider2D& collider);
		static void Submit(TransformComponent& transform, CircleCollider2D& collider);
		static void Submit(TransformComponent& transform, BoxCollider2D& collider);

		static void DepricatedOnStep();


		static bool CheckCollision(const TransformComponent& transform_a, const CircleCollider2D& collider_a, const TransformComponent& transform_b, const CircleCollider2D& collider_b);
		static bool CheckCollision(const TransformComponent& transform_a, const CircleCollider2D& collider_a, const TransformComponent& transform_b, const BoxCollider2D& collider_b);
		static bool CheckCollision(const TransformComponent& transform_a, const BoxCollider2D& collider_a, const TransformComponent& transform_b, const BoxCollider2D& collider_b);
	};

	inline void Physics2D::Clear()
	{
		m_CircleColliders.clear();
		m_BoxColliders.clear();
		m_StaticCircleColliders.clear();
		m_StaticBoxColliders.clear();
	}

	

	inline void Physics2D::Submit(Entity entity, TransformComponent& transform, Rigidbody2D& rigidbody, CircleCollider2D& collider)
	{
		m_CircleColliders.push_back(std::make_tuple(entity, transform, rigidbody, collider));
	}

	inline void Physics2D::Submit(Entity entity, TransformComponent& transform, Rigidbody2D& rigidbody, BoxCollider2D& collider)
	{
		m_BoxColliders.push_back(std::make_tuple(entity, transform, rigidbody, collider));
	}

	inline void Physics2D::Submit(TransformComponent& transform, CircleCollider2D& collider)
	{
		m_StaticCircleColliders.push_back(std::make_tuple(transform, collider));
	}

	inline void Physics2D::Submit(TransformComponent& transform, BoxCollider2D& collider)
	{
		m_StaticBoxColliders.push_back(std::make_tuple(transform, collider));
	}

	inline void Physics2D::DepricatedOnStep()
	{
		for (auto& [entity, transform_component, rigidbody, collider] : m_CircleColliders)
		{
			Rigidbody2D& rbref = entity.GetComponent<Rigidbody2D>();

			if (rbref.useGravity)
				rbref.velocity.y -= 9.8f * Time::DeltaTime();

			Transform transform = entity;

			transform.SetLocalPosition(transform.GetLocalPosition() + rbref.velocity * Time::DeltaTime());

			for (auto& [static_transform, static_collider] : m_StaticCircleColliders)
			{
				if (CheckCollision(transform.GetLocalPosition(), collider, static_transform, static_collider))
				{
					Vector2 apos = static_cast<Vector2>(transform.GetLocalPosition()) + collider.offset;
					Vector2 bpos = static_cast<Vector2>(static_transform.Position) + static_collider.offset;

					float distance = Vector2::Distance(apos,bpos);
					float distancesqrt = Vector2::SqrLength(apos - bpos);

					float overlap = .5f * (distance - collider.radius - static_collider.radius);

					Vector2 normal = (apos - bpos) / distance;
					Vector2 normalt = (bpos - apos) / distance;
					Vector2 tangent = Vector2(-normalt.y, normalt.x);
					
					Vector2 dot_tangent = Vector2::Dot(rbref.velocity, tangent) * tangent;

					rbref.velocity = tangent * dot_tangent;

					//rbref.velocity = Vector2::Zero();
					transform.SetLocalPosition(transform.GetLocalPosition() - (overlap * normal));
					transform.SetLocalPosition(transform.GetLocalPosition() -  rbref.velocity * Time::DeltaTime());
				}
			}
			for (auto& [static_transform, static_collider] : m_StaticBoxColliders)
			{
				if (CheckCollision(transform.GetLocalPosition(), collider, static_transform, static_collider))
				{
					rbref.velocity = Vector2::Zero();
					transform.SetLocalPosition(transform.GetLocalPosition() - rbref.velocity * Time::DeltaTime());
				}
			}
		}

		for (auto& [entity, transform_component, rigidbody, collider] : m_BoxColliders)
		{
			if (rigidbody.useGravity)
			rigidbody.velocity.y -= 9.8f * Time::DeltaTime();

			Transform transform = entity;
			transform.SetLocalPosition(transform_component.Position + rigidbody.velocity * Time::DeltaTime());
			for (auto& [static_transform, static_collider] : m_StaticCircleColliders)
			{
				if (CheckCollision(static_transform, static_collider, transform_component, collider))
				{
					rigidbody.velocity = Vector2::Zero();
					transform.SetLocalPosition(transform_component.Position - rigidbody.velocity * Time::DeltaTime());
				}
			}
			for (auto& [static_transform, static_collider] : m_StaticBoxColliders)
			{
				if (CheckCollision(transform_component, collider, static_transform, static_collider))
				{
					rigidbody.velocity = Vector2::Zero();
					transform.SetLocalPosition(transform_component.Position - rigidbody.velocity * Time::DeltaTime());
				}
			}
		}

		Clear();
	}

	inline bool Physics2D::CheckCollision(const TransformComponent& transform_a, const CircleCollider2D& collider_a,
		const TransformComponent& transform_b, const CircleCollider2D& collider_b)
	{
		if (collider_a.isTrigger || collider_b.isTrigger)
			return false;

		if (Vector2::Distance(static_cast<Vector2>(transform_a.Position) + collider_a.offset, static_cast<Vector2>(transform_b.Position) + collider_b.offset) < collider_a.radius + collider_b.radius)
			return true;

		return false;
	}

	inline bool Physics2D::CheckCollision(const TransformComponent& transform_a, const CircleCollider2D& collider_a,
		const TransformComponent& transform_b, const BoxCollider2D& collider_b)
	{
		if (collider_a.isTrigger || collider_b.isTrigger)
			return false;
		Vector2 box_half_extents = collider_b.size * 0.5f;
		Vector2 box_center = static_cast<Vector2>(transform_b.Position) + collider_b.offset;
		Vector2 difference = static_cast<Vector2>(transform_a.Position) + collider_a.offset - box_center;
		Vector2 clamped = Vector2::Clamp(difference, -box_half_extents, box_half_extents);
		Vector2 closest = box_center + clamped;
		difference = closest - (static_cast<Vector2>(transform_a.Position) + collider_a.offset);
		if (Vector2::SqrLength(difference) < collider_a.radius * collider_a.radius)
			return true;
		return false;
	}

	bool PointInBox(Vector2 point, Vector2* vertecies);

	inline bool Physics2D::CheckCollision(const TransformComponent& transform_a, const BoxCollider2D& collider_a,
	                                      const TransformComponent& transform_b, const BoxCollider2D& collider_b)
	{
		if (collider_a.isTrigger || collider_b.isTrigger)
			return false;
		
		// Check collision between two struct BoxCollider2D { Vector2 size, Vector2 offset } and it's affect by transform rotation and transform.Rotation is a Quaternion bruh

		// Get rotation matrix
		Matrix4x4 rotation_matrix_a = Matrix4x4::Rotation(transform_a.Rotation);
		Matrix4x4 rotation_matrix_b = Matrix4x4::Rotation(transform_b.Rotation);

		// Calculate vertices of collider a
		Vector2 half_extents_a = collider_a.size * 0.5f;
		Vector2 vertices_a[4] = { Vector2(-half_extents_a.x, -half_extents_a.y), Vector2(-half_extents_a.x, half_extents_a.y), Vector2(half_extents_a.x, half_extents_a.y), Vector2(half_extents_a.x, -half_extents_a.y) };
		for (uint32_t i = 0; i < 4; i++)
		{
			vertices_a[i] = rotation_matrix_a * vertices_a[i];
			vertices_a[i] += static_cast<Vector2>(transform_a.Position) + collider_a.offset;
		}

		// Calculate vertices of collider b
		Vector2 half_extents_b = collider_b.size * 0.5f;
		Vector2 vertices_b[4] = { Vector2(-half_extents_b.x, -half_extents_b.y), Vector2(-half_extents_b.x, half_extents_b.y), Vector2(half_extents_b.x, half_extents_b.y), Vector2(half_extents_b.x, -half_extents_b.y) };
		for (uint32_t i = 0; i < 4; i++)
		{
			vertices_b[i] = rotation_matrix_b * vertices_b[i];
			vertices_b[i] += static_cast<Vector2>(transform_b.Position) + collider_b.offset;
		}

		// Check if two collider is intersecting
		for (uint32_t i = 0; i < 4; i++)
		{
			if (PointInBox(vertices_a[i], vertices_b))
				return true;
			if (PointInBox(vertices_b[i], vertices_a))
				return true;
		}

		return false;
	}
}
