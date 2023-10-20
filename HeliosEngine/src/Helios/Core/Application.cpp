#include "Application.h"

#include "AssetManager.h"
#include "Profiler.h"
#include "Time.h"
#include "Helios/Graphics/Renderer.h"
#include "Helios/Graphics/Renderer2D.h"
#include "Helios/Graphics/GizmosRenderer.h"
#include "Helios/Input/InputManager.h"
#include "Helios/Physics/Physics2D.h"
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
		//m_Window->SetVSync(true);

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

	std::condition_variable s_PhysicsCV;
	std::mutex s_PhysicsMutex;
	bool s_PhysicsEnabled = false;
	bool s_PhysicsFinished = false;

	void RunPhysics(const bool* isRunning)
	{
		while (*isRunning)
		{
			{
				std::unique_lock<std::mutex> lock(s_PhysicsMutex);
				s_PhysicsCV.wait(lock, [] { return s_PhysicsEnabled; });
			}

			//std::cout << "Physics thread sync" << std::endl;

			float fixedTimestep = 1.f / 60.f;

			static float m_accumulator = 0.0f;
			m_accumulator += Time::deltaTime();

			while (m_accumulator >= fixedTimestep)
			{
				//std::cout << "Physics step" << std::endl;
				Physics2D::OnStep();
				m_accumulator -= fixedTimestep;
			}

			{
				s_PhysicsFinished = true;
				s_PhysicsCV.notify_one();
			}
		}

		s_PhysicsFinished = true;
		s_PhysicsCV.notify_one();
	}

	void Application::Run()
	{
		std::thread m_PhysicsThread = std::thread(RunPhysics, &m_Running);

		Time::frameUpdate(); // Update time before first frame to avoid huge delta time

		try {
			while (m_Running)
			{
				HL_PROFILE_FRAME_BEGIN();
				//std::cout << "Frame" << std::endl;

				Time::frameUpdate();

				m_Window->BeginFrame();

				OnUpdate();
				s_PhysicsEnabled = true;
				s_PhysicsCV.notify_one();
				OnRender();

				m_Window->EndFrame();

				InputManager::s_MouseWheelDelta = 0.0f;

				HL_PROFILE_BEGIN("Wait for physics thread");

				{
					std::unique_lock<std::mutex> lock(s_PhysicsMutex);
					s_PhysicsCV.wait(lock, [] { return s_PhysicsFinished; });
					s_PhysicsFinished = false;
				}
				s_PhysicsEnabled = false;

				HL_PROFILE_END();

				HL_PROFILE_BEGIN("AssetManager::OnUpdate()");
				AssetManager::OnUpdate();
				HL_PROFILE_END();

				HL_PROFILE_FRAME_END();
				GraphicalWindow::PollEvents();
			}
		}
		catch (Helios::HeliosException e) {
			e.what(false);
		}
		
		s_PhysicsEnabled = true;
		s_PhysicsCV.notify_one();

		m_PhysicsThread.join();
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
