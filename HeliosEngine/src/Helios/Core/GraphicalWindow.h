#pragma once
#include "pch.h"
#include "Base.h"
#include "Asserts.h"
#include "Helios/Events/Event.h"
#include "Helios/Graphics/GraphicsContext.h"

namespace Helios
{
	namespace WindowStyles
	{
		enum Value : bitmask_t
		{
			None		= 0,
			Decorated	= BIT(0),
			Resizable	= BIT(1),
			//Minimized	= BIT(2),
			Maximized	= BIT(3),
			Show		= BIT(4),
			AlwaysOnTop	= BIT(5),
		};

		inline Value operator | (Value lhs, Value rhs)
		{
			using T = std::underlying_type_t <Value>;
			return static_cast<Value>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		inline Value& operator |= (Value& lhs, Value rhs)
		{
			lhs = lhs | rhs;
			return lhs;
		}
	}

	class HELIOS_API GraphicalWindow
	{
	public:
#pragma region Window Styles

		struct Specifications
		{
			std::string title;
			uint32_t width;
			uint32_t height;
			WindowStyles::Value style = WindowStyles::Decorated | WindowStyles::Resizable | WindowStyles::Show;
		};
	public: // Constructors & Destructor
	 	static Ref<GraphicalWindow> Create();
	
		virtual ~GraphicalWindow() = default;

		using EventCallbackFun = std::function<void(Event&)>;

		void SetEventCallback(const EventCallbackFun& callback) { m_EventCallback = callback; }

#pragma region Window lifecycle methods

		virtual bool Create(Specifications specs) = 0;
		virtual void Destroy() = 0;
		virtual ImGuiContext* CreateImGuiContext() = 0;
		virtual void ImpImGui() = 0;
#pragma endregion

#pragma region Getters

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual GraphicsContext& GetContext() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual ImGuiContext* GetImGuiContext() const = 0;

#pragma endregion 

#pragma region Setters
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetTitle(const std::string& title) = 0;
#pragma endregion

#pragma region State management methods

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Update() = 0;

#pragma endregion

#pragma region Window management methods

		virtual void Show() = 0;
		virtual bool IsFocused() = 0;

#pragma endregion

#pragma region Static Methods
		static void PollEvents()
		{
#if defined(HELIOS_PLATFORM_WINDOWS) || defined(HELIOS_PLATFORM_LINUX) || defined(HELIOS_PLATFORM_MACOS)
			glfwPollEvents();
#else
#endif
		}
#pragma endregion
	protected:
		EventCallbackFun m_EventCallback;
	};
}
