#pragma once
#include "Event.h"
#include "Helios/Logger/Logger.h"

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

	class LogEvent : public Event
	{
		public:
			LogEvent(Logger::LogLevel level, std::string_view message) : m_Level(level), m_Message(message) { }
			EVENT_TYPE(Log)
			[[nodiscard]] Logger::LogLevel GetLevel() const { return m_Level; }
			[[nodiscard]] std::string_view GetMessage() const { return m_Message; }
		private:
			std::string_view m_Message;
			Logger::LogLevel m_Level;
	};
}
