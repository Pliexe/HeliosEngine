#include "Physics2D.h"

#include "Box2D/Physics2DBox2D.h"

namespace Helios
{
	inline IPhysics2D& Physics2D::GetInstance()
	{
		switch (s_implementationType)
		{
		case ImplementationType::HeliosPhysics:
		{
			HL_ASSERT_EXCEPTION(false, "HeliosPhysics not implemented yet!");
			break;
		}
		case ImplementationType::Box2D:
		{
			static Physics2DBox2D instance;
			return instance;
		}
		}
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
