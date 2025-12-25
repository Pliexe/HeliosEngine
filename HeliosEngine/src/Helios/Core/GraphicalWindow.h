#pragma once
#include "Helios/Graphics/GraphicsDevice.h"
#include "pch.h"
#include "Base.h"
#include "Asserts.h"
#include "Helios/Events/Event.h"
#include "Helios/Graphics/GraphicsContext.h"
#include "Helios/Utils/Message.h"
#include "Helios/Math/Vector.h"
#include "Helios/Graphics/NativeCommandList.h"

namespace Helios
{
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
	 	static Unique<GraphicalWindow> CreateScoped();

		GraphicalWindow() { if (s_MainWindow == nullptr) s_MainWindow = this; }
	
		virtual ~GraphicalWindow() = default;

		using EventCallbackFun = std::function<void(Event&)>;

		void SetEventCallback(const EventCallbackFun& callback) { m_EventCallback = callback; }

#pragma region Window lifecycle methods

		virtual bool Create(Specifications specs) = 0;
		// Send close signal
		virtual void Close() = 0;
		// Destroy the window
		virtual void Destroy() = 0;
		virtual ImGuiContext* CreateImGuiContext() = 0;
		virtual void ImpImGui() = 0;
#pragma endregion

#pragma region Getters

		static const GraphicalWindow* GetMainWindow() { return s_MainWindow; }

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual Point GetPosition() const = 0;

		virtual std::string GetWindowTitle() const = 0;

		virtual GraphicsContext& GetContext() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual ImGuiContext* GetImGuiContext() const = 0;

		virtual std::string GetTitle() const = 0;

		virtual bool VSyncEnabled() const  = 0;


#pragma endregion 

#pragma region Setters
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetTitle(const std::string& title) = 0;

		void SetPosition(int32_t x, int32_t y) { SetPosition({ x, y }); }
		virtual void SetPosition(const Point& pos) = 0;
#pragma endregion

#pragma region State management methods

		// virtual NativeCommandList& BeginFrame() = 0;
		// virtual void EndFrame() = 0;

        virtual void DrawImGui(NativeCommandListImmediate& commandList) = 0;
		virtual void NewImGuiFrame() = 0;

		virtual void Update() = 0;

#pragma endregion

#pragma region Window management methods

		virtual bool IsMaximized() const = 0;
		virtual void Maximize() = 0;
		virtual void Minimize() = 0;
		virtual void Restore() = 0;

		virtual void Show() = 0;
		virtual bool IsFocused() = 0;

		virtual Message::Result ShowMessage(const std::string& title, const std::string& message, Message::Flags type = Message::Flags::Ok | Message::Flags::IconInformation) const = 0;

#pragma endregion

#pragma region Static Methods

		static void Message(const std::string& title, const std::string& message);

		static void PollEvents();
#pragma endregion
	protected:
		EventCallbackFun m_EventCallback;

		inline static GraphicalWindow* s_MainWindow = nullptr;

		std::string m_Title;
	};
}
