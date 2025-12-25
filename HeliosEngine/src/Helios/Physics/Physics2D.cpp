#include "Physics2D.h"

#include "Box2D/Physics2DBox2D.h"

namespace Helios
{

	class Physics2DNone : public IPhysics2D
	{
	public:
		Physics2DNone() {}
		Physics2DNone(const Physics2DNone&) = delete;

		CircleBody2D* CreateCircle(Entity entity, CircleCollider2D& collider) override { return nullptr; }
		BoxBody2D* CreateBox(Entity entity, BoxCollider2D& collider) override { return nullptr; }

		void OnStep() override {}
	};

	static IPhysics2D* s_Instance = new Physics2DNone();

	IPhysics2D& Physics2D::GetInstance()
	{
		return *s_Instance;
		/*switch (s_implementationType)
		{
		case ImplementationType::None:
		{
			static Physics2DNone instance;
			return instance;
		}
		case ImplementationType::HeliosPhysics:
		{
			HL_ASSERT(false, "HeliosPhysics not implemented yet!");
			break;
		}
		case ImplementationType::Box2D:
		{
			static Physics2DBox2D instance;
			return instance;
		}
		}*/
	}

	void Physics2D::Init(ImplementationType type)
	{
		s_implementationType = type;

		switch (s_implementationType)
		{
		case ImplementationType::None:
		{
			delete s_Instance;
			s_Instance = new Physics2DNone();
			break;
		}
		case ImplementationType::HeliosPhysics:
		{
			HL_ASSERT(false, "HeliosPhysics not implemented yet!");
			break;
		}
		case ImplementationType::Box2D:
		{
			delete s_Instance;
			s_Instance = new Physics2DBox2D();
			break;
		}
		}
	}

	void Physics2D::Shutdown()
	{
		s_implementationType = ImplementationType::None;

		delete s_Instance;
		s_Instance = new Physics2DNone();
	}

	bool PointInBox(Vector2 point, Vector2* vertecies)
	{
		Vector2 AB = vertecies[1] - vertecies[0];
		Vector2 AM = point - vertecies[0];
		float ABAM = Vector2::Dot(AB, AM);
		if (ABAM < 0.0f)
			return false;
		float ABAB = Vector2::Dot(AB, AB);
		if (ABAM > ABAB)
			return false;
		float AMAM = Vector2::Dot(AM, AM);
		if (AMAM > ABAB)
			return false;
		return true;
	}

}
