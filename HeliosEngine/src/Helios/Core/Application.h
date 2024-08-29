#pragma once
#include "pch.h"

#include "Window.h"
#include "Helios/Graphics/Graphics.h"
#include "GraphicalWindow.h"
#include "Helios/Events/Events.h"

int main(int,char**);

namespace Helios
{
	class HELIOS_API Application
	{
	public:
		struct Specifications : GraphicalWindow::Specifications
		{
			Graphics::API graphicsAPI;
		};

		Application(Specifications specs);
		~Application() { m_Instance = nullptr; }

		Scope<GraphicalWindow>& GetWindow() { return m_Window; }

		void Initialize(Specifications specs);
		void Quit();

		// Getters
		bool IsRunning() const;
		static Application& GetInstance() { return *m_Instance; }

	protected:
		void Run();


		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;

		std::string m_applicationName = "Game made with Helios";

	private:
	private:

		void OnEvent(Event& event);

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);

		Scope<GraphicalWindow> m_Window = nullptr;

		bool m_Running = true;

		inline static Application* m_Instance = nullptr;

		friend class GraphicalWindow;
		friend class Win32GraphicalWindow;
		friend class VkContext;

		friend int ::main(int,char**);
	};

	Application* CreateApplication(int argc, char** argv);
}
