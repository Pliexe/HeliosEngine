#pragma once

#include "pch.h"

namespace Helios
{
	enum class EventType
	{
		None = 0, WindowClose, WindowResize
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
	};;

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : m_Event(event) { }

		template<typename T, typename F>
		std::enable_if_t< std::is_base_of_v<Event, T>, bool>
		Dispatch(const F& func)
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