#pragma once

#include "pch.h"

namespace Helios
{
	enum class EventType
	{
		None = 0, WindowClose, WindowResize,

		SceneDestroyed = 10,

		SceneEntityCreated = 20, SceneEntityDestroyed,

		BehaviourEnable = 100, BehaviourDisable,
	};

	class Event
	{
	public:
		virtual ~Event() = default;
		[[nodiscard]] virtual EventType GetType() const = 0;
	private:
		bool m_Handled = false;

		friend class EventDispatcher;
	};

#define EVENT_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
						 virtual EventType GetType() const override { return GetStaticType(); }

	class GraphicalWindow;
	class WindowEvent : public Event
	{
	public:
		WindowEvent(GraphicalWindow& window) : m_Window(window) { }
		[[nodiscard]] GraphicalWindow& GetWindow() const { return m_Window; }
	protected:
		GraphicalWindow& m_Window;
	};

	namespace Scripting {
		class Behaviour;
		class BehaviourEvent : public Event
		{
		public:
			BehaviourEvent(Behaviour& behaviour) : m_Behaviour(behaviour) { }
			[[nodiscard]] Behaviour& GetBehaviour() const { return m_Behaviour; }
		protected:
			Behaviour& m_Behaviour;
		};
	}

	class EventDispatcher
	{
	public:
		explicit EventDispatcher(Event& event) : m_Event(event) { }

		template<typename T, typename F>
		bool Dispatch(const F& func)
		requires (std::is_base_of_v<Event, T>)
		{
			if (m_Event.GetType() == T::GetStaticType())
			{
				m_Event.m_Handled |= func(static_cast<T&>(m_Event));
				return true;
			} else return false;
		}

	private:
		Event& m_Event;
	};
}