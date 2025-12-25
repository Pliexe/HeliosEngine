#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/CommandAllocator.h"
#include "Application.h"

#include "AssetManager.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Resources/Shader.h"
#include "Profiler.h"
#include "Time.h"
#include "Helios/Input/InputManager.h"
#include "Helios/Physics/Physics2D.h"
#include "Helios/Resources/ResourceRegistry.h"
#include "Helios/Graphics/Sampler.h"
#include "Helios/Resources/ShaderModule.h"

#include "Helios/Graphics/GPURenderPass.h"

namespace Helios
{
	Application::Application(Specifications specs)
	{
		HL_ASSERT( !m_Instance, "Application already exists!" );
		m_Instance = this;
		Initialize(specs);
	}

    Application::~Application()
    {
		Sampler::Destroy();
		
		Scripting::ShutdownScriptManager();
		SceneRegistry::Shutdown();
		// Renderer2D::Shutdown();
		// Renderer::Shutdown();
		// GizmosRenderer::Shutdown();
		ResourceRegistry::Shutdown();

		m_Window.reset();
		Graphics::Shutdown();
		m_Instance = nullptr;

		std::cout << "Application Shutdown!" << std::endl;
    }

    void Application::Initialize(Specifications specs)
    {
		// Make sure locale is UTF-8 (call once per program, ideally)
		std::setlocale(LC_ALL, "");

		HL_EXCEPTION(!Graphics::Initialize(specs.graphicsAPI), "Failted to intialize graphics!");
		
		m_Window = std::move(GraphicalWindow::CreateScoped());

		std::function<void(Event&)> callback = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		m_Window->SetEventCallback(callback);
		m_Window->Create({ specs.title, specs.width, specs.height, specs.style });
		m_Window->SetVSync(false);
		
// 		if (!Renderer2D::Init()) HL_EXCEPTION(true, "Failed to initialize Renderer2D")
// 		if (!Renderer::Init()) HL_EXCEPTION (true, "Failed to initialize Renderer")


#ifdef HELIOS_EDITOR
		MeshGenerator::InitializeMeshTypeMapping();
		// if (!GizmosRenderer::Init()) HL_EXCEPTION(true, "Failed to initialize GizmosRenderer")
#endif

		std::ios::sync_with_stdio(false);

#pragma region Init Counter

		Time::internal_init();

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
			m_accumulator += Time::DeltaTime();

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

		Time::internal_init(); // Reset time before first frame to avoid huge delta time

		try {
			uint64 frame = 0;
			while (m_Running)
			{
				std::printf("Frame %lu\n", frame++);
				GraphicalWindow::PollEvents();
				
				HL_PROFILE_FRAME_BEGIN();

				Time::internal_frame_update();

				Graphics::Begin();

				auto& commandList = Graphics::GetMainCommandList();

				OnUpdate();
				s_PhysicsEnabled = true;
				s_PhysicsCV.notify_one();
				Graphics::GetMainRenderGraph()->Execute();
				OnRender(commandList);

				Graphics::End();

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

				// const double targetFrameTime = 1.0 / 60.0; // 16.67 ms

				// double deltaTime = Time::DeltaTime(); // seconds elapsed this frame

				// double timeToSleep = targetFrameTime - deltaTime;

				// if (timeToSleep > 0)
				// {
				// 	std::this_thread::sleep_for(std::chrono::duration<double>(timeToSleep));
				// }

			}
		}
		catch (Helios::HeliosException e) {
			Helios::ShowMessage("Helios Exception", e.what());
		}
		
		s_PhysicsEnabled = true;
		s_PhysicsCV.notify_one();

		m_PhysicsThread.join();

		m_Window->GetContext().WaitAllSync();
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
#ifdef HELIOS_EDITOR
		// m_Window->BeginFrame();
		// OnUpdate();
		// OnRender();
		// m_Window->EndFrame();
#endif
		m_Window->GetContext().ResizeSwapchain({ event.GetWidth(), event.GetHeight() });
		return true;
	}

	void Application::Quit() { m_Running = false; }

}
