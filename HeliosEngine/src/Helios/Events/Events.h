#pragma once
#include "Event.h"

namespace Helios
{
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;
		EVENT_TYPE(WindowClose)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) { }
		EVENT_TYPE(WindowResize)
		[[nodiscard]] uint32_t GetWidth() const { return m_Width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_Height; }
	private:
		uint32_t m_Width, m_Height;
	};
}
