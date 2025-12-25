#pragma once

#include "Event.h"

namespace Helios
{
	class EntityContainer;
	class SceneDestroyedEvent : public Event
	{
	public:
		SceneDestroyedEvent() = default;
		EVENT_TYPE(SceneDestroyed)
	};
	
    class SceneEntityCreatedEvent : public Event
	{
	public:
		SceneEntityCreatedEvent() = default;
		EVENT_TYPE(SceneEntityCreated)
    protected:
	};

	class SceneEntityDestroyedEvent : public Event
	{
	public:
		SceneEntityDestroyedEvent() = default;
		EVENT_TYPE(SceneEntityDestroyed)
	};
}