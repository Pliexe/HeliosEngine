#pragma once
#include <box2d/box2d.h>

#include "Helios/Physics/Physics2D.h"

namespace Helios
{
	static std::mutex s_PhysicsMutex;

	class HELIOS_API IBodyBox2D : public IBody2D
	{
	protected:
		b2Body* m_Body;
		b2Fixture* m_Fixture;
		Vector2 m_Offset;
		IBodyBox2D(b2Body* body, b2Fixture* fixture, const Vector2& offset)
			: m_Body(body), m_Fixture(fixture), m_Offset(offset) {}

	public:
		IBodyBox2D() = delete;
		IBodyBox2D(const IBodyBox2D&) = delete;

		void SetPosition(const Vector2& position) override
		{
			s_PhysicsMutex.lock();
			m_Body->SetTransform(b2Vec2(position.x + m_Offset.x, position.y + m_Offset.y), m_Body->GetAngle());
			m_Body->SetAwake(true);
			s_PhysicsMutex.unlock();
		}

		void SetRotation(float rotation) override
		{
			s_PhysicsMutex.lock();
			m_Body->SetTransform(m_Body->GetPosition(), rotation);
			s_PhysicsMutex.unlock();
		}

		void SetOffset(const Vector2& offset) override { m_Offset = offset; }

		Vector2 GetPosition() override { return Vector2(m_Body->GetPosition().x, m_Body->GetPosition().y) - m_Offset; }
		Vector2 GetVelocity() override { return Vector2(m_Body->GetLinearVelocity().x, m_Body->GetLinearVelocity().y); }
		float GetRotation() override { return m_Body->GetAngle(); }

		friend class Physics2DBox2D;
	};

	class HELIOS_API CircleBodyBox2D : public IBodyBox2D, public ICircleBody2D
	{
	private:
		CircleBodyBox2D(b2Body* body, b2Fixture* fixture, const Vector2& offset)
			: IBodyBox2D(body, fixture, offset) {}
	public:

		void SetRadius(float radius) override
		{
			s_PhysicsMutex.lock();
			m_Fixture->GetShape()->m_radius = radius;
			s_PhysicsMutex.unlock();
		}

	public:
		friend class Physics2DBox2D;
	};

	class HELIOS_API BoxBodyBox2D : public IBodyBox2D, public IBoxBody2D
	{
	private:
		BoxBodyBox2D(b2Body* body, b2Fixture* fixture, const Vector2& offset)
			: IBodyBox2D(body, fixture, offset) {}
	public:
		void SetSize(const Vector2& size) override
		{
			s_PhysicsMutex.lock();
			b2PolygonShape* shape = static_cast<b2PolygonShape*>(m_Fixture->GetShape());
			shape->SetAsBox(size.x / 2.0f, size.y / 2.0f);
			s_PhysicsMutex.unlock();
		}
	public:
		friend class Physics2DBox2D;
	};

	class Physics2DBox2D : public IPhysics2D
	{
	private:
		
		b2World* m_World;

	public:

		Physics2DBox2D()
		{
			//m_World = new b2World(b2Vec2(0.0f, -0.f));
			m_World = new b2World(b2Vec2(0.0f, -9.81f));
		}

		Physics2DBox2D(const Physics2DBox2D&) = delete;

		CircleBody2D* CreateCircle(Entity entity, CircleCollider2D& collider) override
		{
			s_PhysicsMutex.lock();
			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			b2BodyDef bodyDef;

			if (entity.HasComponent<Rigidbody2D>())
			{
				Rigidbody2D& rigidbody = entity.GetComponent<Rigidbody2D>();

				switch (rigidbody.type)
				{
					case BodyType::Static:
						bodyDef.type = b2_staticBody;
						break;
					case BodyType::Dynamic:
						bodyDef.type = b2_dynamicBody;
						break;
					case BodyType::Kinematic:
						bodyDef.type = b2_kinematicBody;
						break;
				}
			}
			else
			{
				bodyDef.type = b2_staticBody;
			}

			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = transform.Rotation.euler().z;

			b2Body* body = m_World->CreateBody(&bodyDef);

			b2CircleShape circle;
			circle.m_radius = collider.radius;
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;

			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;

			fixtureDef.restitution = 0.5f;
			fixtureDef.filter.categoryBits = 0x0001;
			fixtureDef.filter.maskBits = 0xFFFF;
			fixtureDef.isSensor = collider.isTrigger;
			fixtureDef.restitutionThreshold = 0.0f;

			body->CreateFixture(&fixtureDef);

			s_PhysicsMutex.unlock();

			return (CircleBody2D*) (new CircleBodyBox2D(body, body->GetFixtureList(), collider.offset));
		}

		BoxBody2D* CreateBox(Entity entity, BoxCollider2D& collider) override
		{
			s_PhysicsMutex.lock();

			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			b2BodyDef bodyDef;
			if (entity.HasComponent<Rigidbody2D>())
			{
				Rigidbody2D& rigidbody = entity.GetComponent<Rigidbody2D>();
				switch (rigidbody.type)
				{
					case BodyType::Static:
						bodyDef.type = b2_staticBody;
						break;
					case BodyType::Dynamic:
						bodyDef.type = b2_dynamicBody;
						break;
					case BodyType::Kinematic:
						bodyDef.type = b2_kinematicBody;
						break;
				}
			}
			else
			{
				bodyDef.type = b2_staticBody;
			}

			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			//bodyDef.angle = transform.Rotation.euler().z;

			b2Body* body = m_World->CreateBody(&bodyDef);
			b2PolygonShape box;
			box.SetAsBox(collider.size.x / 2.0f, collider.size.y / 2.0f);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &box;
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.3f;
			body->CreateFixture(&fixtureDef);

			s_PhysicsMutex.unlock();

			return (BoxBody2D*) new BoxBodyBox2D(body, body->GetFixtureList(), collider.offset);
		}

		void OnStep() override
		{
			s_PhysicsMutex.lock();
			m_World->Step(1.0f / 60.0f, 8, 3);
			s_PhysicsMutex.unlock();
			
		}
	};
}
