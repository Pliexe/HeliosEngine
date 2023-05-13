#include "Application.h"

#include "Time.h"
#include "Helios/Graphics/Renderer.h"
#include "Helios/Graphics/Renderer2D.h"
#include "Helios/Graphics/GizmosRenderer.h"
#include "Helios/Input/InputManager.h"
#include "Platform/Windows/Win32GraphicalWindow.h"

namespace Helios
{
	Application::Application(Specifications specs)
	{
		HL_ASSERT_EXCEPTION( !m_Instance, "Application already exists!" );
		m_Instance = this;
		Initialize(specs);
	}

	void Application::Initialize(Specifications specs)
	{
		Graphics::m_api = specs.graphicsAPI;
#ifdef HELIOS_PLATFORM_WINDOWS
		m_Window = CreateScope<Win32GraphicalWindow>();
#endif
		std::function<void(Event&)> callback = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		m_Window->SetEventCallback(callback);
		m_Window->Create({ specs.title, specs.width, specs.height, WindowStyles::Decorated | WindowStyles::Resizable });

		if (!Renderer2D::Init()) HL_EXCEPTION(true, "Failed to initialize Renderer2D")
		if (!Renderer::Init()) HL_EXCEPTION (true, "Failed to initialize Renderer")
#ifdef HELIOS_EDITOR
		if (!GizmosRenderer::Init()) HL_EXCEPTION(true, "Failed to initialize GizmosRenderer")
#endif

			std::ios::sync_with_stdio(false);

#pragma region Init Counter

		Time::Init();

#pragma endregion
	}

	bool Application::IsRunning() const { return m_Running; }

	void Application::Run()
	{
		while(m_Running)
		{
			Time::frameUpdate();

			m_Window->BeginFrame();

			OnUpdate();

			m_Window->EndFrame();

			InputManager::s_MouseWheelDelta = 0.0f;

			GraphicalWindow::PollEvents();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		Quit();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		return false;
	}

	void Application::Quit() { m_Running = false; }

}
