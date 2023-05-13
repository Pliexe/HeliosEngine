#include "Application.h"
#include "Helios/Core/Application.h"

#include <iostream>
#include <windows.h>

#include <Helios.h>
#include <Helios/Resources/Color.h>
#include <Helios/Graphics/Renderer2D.h>

#include <Helios/Utils/ErrorHandling.h>

#include <d2d1_3.h>

#include "Helios/Utils/ErrorHandling.h"
#include <filesystem>

#include <Panels/ProjectExplorer.h>
#include "ProjectManager.h"

#include "Icons.h"

#include "HeliosEditor_Macros.h"
#include <imgui_impl_win32.cpp>
#include <imgui_impl_dx11.cpp>
#include "Helios/Translation/Matrix.h"
#include <Helios/Translation/Quaternion.h>
#include <sstream>
#include <Helios/Graphics/Framebuffer.h>

#include "AssetRegistry.h"
#include <Helios/Resources/Shader.h>
#include <Helios/Input/InputManager.h>
#include <Helios/Input/KeyCodes.h>

#include "ApplicationStyle.h"
#include "Graphics/ProfilerTimeline.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Graphics/GizmosRenderer.h"

#include "Graphics/StackedGraph.h"
#include "Helios/Scene/EditorCamera.h"

#include "DirectXMath.h"
#include "GUI/Toolbar.h"
#include "Platform/Windows/Win32GraphicalWindow.h"


static std::filesystem::path currentScene;
StartupConfig startupConfig;

using namespace Helios;

static EditorMode currentMode = EditorMode::Editor;

bool isGameSceneActive = false;
bool isEditorSceneActive = false;

struct TransformVertex
{
	Vector3 position;
	Color color;
};

int ValidateInit() {

	try {
		Project::AttemptLoad();
		SetCurrentPath(Project::GetAssetsPath());
	}
	catch (int code) {
		return code;
	}

	return 0;
}

#include "Platform/Windows/Win32GraphicalWindow.h"

bool generate_proj = false;
inline std::filesystem::path PROJECT_PATH;

Helios::Application* Helios::CreateApplication(int argc, char** argv)
{
	Application::Specifications specs;
	specs.graphicsAPI = Graphics::API::Direct3D11;
	specs.width = 1280;
	specs.height = 720;
	

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
		{
			std::cout << "Helios Editor" << std::endl;
			std::cout << "Usage: HeliosEditor.exe [options]" << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "\t-h - Show this help message" << std::endl;
			std::cout << "\t-project <path> - Open project at <path>" << std::endl;
			exit(0);  // NOLINT(concurrency-mt-unsafe)
		}

		if (strcmp(argv[i], "-project") == 0 && (i+1 < argc))
		{
			PROJECT_PATH = argv[i+1];

			if(std::filesystem::exists(PROJECT_PATH) && PROJECT_PATH.has_extension() && PROJECT_PATH.extension() == ".helios")
			{
				PROJECT_PATH = PROJECT_PATH.parent_path();
			}
		}

		if (strcmp(argv[i], "-generate") == 0) generate_proj = true;
	}

	specs.title = "Helios Editor";
	specs.style = WindowStyles::Decorated | WindowStyles::Resizable;

	return new Helios::HeliosEditor(specs);
}

namespace Helios
{
	HeliosEditor::HeliosEditor(const Specifications& specs): Application(specs)
	{
		/*GraphicalWindow::Specifications windowSpecs;
			windowSpecs.title = "Loading...";
			windowSpecs.width = 1280;
			windowSpecs.height = 720;
			windowSpecs.style = WindowStyles::Show;
			Win32GraphicalWindow window;
			window.Create(windowSpecs);
			window.GetContext().UseContext();
			window.GetContext().SwapBuffers();
			window.GetContext().SwapBuffers();
			Sleep(5000);
			window.Destroy();*/

		if (std::filesystem::exists(PROJECT_PATH))
		{
			if (Project::HasProjectFile(PROJECT_PATH))
			{
				Project::LoadProject(PROJECT_PATH, [](std::string cj, float prog) {});
			}
			else if (generate_proj)
			{
				Project::CreateProject(PROJECT_PATH, [](std::string cj, float prog) {});
			}
			else
			{
				std::cout << "The path given does not contain a Helios project. Use -generate if you want to make a new project!" << std::endl;
#ifdef HELIOS_PLATFORM_WINDOWS
				MessageBoxA(nullptr, "The path given does not contain a Helios project.", "Helios Editor", MB_OK | MB_ICONERROR);
#endif
				exit(1);  // NOLINT(concurrency-mt-unsafe)
			}
		}
		else
		{
			if (generate_proj)
			{
				Project::CreateProject(PROJECT_PATH, [](std::string cj, float prog) {});
			}
			else
			{
				std::cout << "The path given does not contain a Helios project. Use -generate if you want to make a new project!" << std::endl;
#ifdef HELIOS_PLATFORM_WINDOWS
				MessageBoxA(nullptr, "The path given does not contain a Helios project.", "Helios Editor", MB_OK | MB_ICONERROR);
#endif
				exit(1);  // NOLINT(concurrency-mt-unsafe)
			}
		}

		GetWindow()->SetTitle("Helios Editor | Project: " + Project::GetProjectSettings().name);

		ImGui::SetCurrentContext(GetWindow()->CreateImGuiContext());
		GetWindow()->ImpImGui();

		InitImGuiStyle();

		GetWindow()->GetContext().UseContext();

		try
		{
			ICON_FOLDER = Texture2D::Create("Resources/Icons/folder.png");
			ICON_FOLDER_EMPTY = Texture2D::Create("Resources/Icons/folder_empty.png");
			ICON_FILE_UNKNOWN = Texture2D::Create("Resources/Icons/unknown_file.png");
			ICON_FILE_TXT = Texture2D::Create("Resources/Icons/text_file.png");
			ICON_FILE_C = Texture2D::Create("Resources/Icons/c_file.png");
			ICON_FILE_H = Texture2D::Create("Resources/Icons/h_file.png");
			ICON_FILE_CPP = Texture2D::Create("Resources/Icons/cpp_file.png");
			ICON_FILE_HPP = Texture2D::Create("Resources/Icons/hpp_file.png");
			ICON_FILE_FONT = Texture2D::Create("Resources/Icons/ttf_file.png");
			ICON_FILE_SCENE = Texture2D::Create("Resources/Icons/scene_file.png");
			ICON_FILE_IMAGE = Texture2D::Create("Resources/Icons/image_file.png");
			ICON_PLAY_WHITE = Texture2D::Create("Resources/Icons/play_white.png");
			ICON_PAUSE_WHITE = Texture2D::Create("Resources/Icons/pause_white.png");
			ICON_STOP_WHITE = Texture2D::Create("Resources/Icons/stop_white.png");
				
		} catch (HeliosException e)
		{
			MessageBoxA(nullptr, "Error loading icons!", std::to_string(e.what()).c_str(), MB_ICONERROR);
			exit(-100);
		}


		if(ICON_FOLDER_EMPTY == nullptr)
			MessageBoxA(nullptr, "Error loading icons!", "ICON_FOLDER_EMPTY == nullptr", MB_ICONERROR);
		
		gameFrame = Framebuffer::Create(300, 300, {
            Framebuffer::Format::R8B8G8A8_UNORM,
            //Framebuffer::Format::R32F,
            Framebuffer::Format::D32F
        });

		profilerGraph = CreateRef<StackedGraph>();

		AssetRegistry::Init();

		using namespace Helios;

		Project::TryLoad();

		panels.push_back(&inspector);
		panels.push_back(&hierarchy);
		panels.push_back(&gameViewPanel);
		panels.push_back(&scenePanel);
		//panels.push_back(&AssetRegistry::Get());

		GetWindow()->Show();
	}

	void HeliosEditor::OnUpdate()
	{
		switch (currentMode)
		{
		case EditorMode::Playing:
			SceneRegistry::OnEditorUpdate();
			break;
		default:
			SceneRegistry::OnEditorUpdate();
			break;
		}

		if (InputManager::IsKeyPressed(HL_KEY_CONTROL) && InputManager::IsKeyPressed(HL_KEY_F10))
		{
			DepricatedApplication::ShowMessage("INFO!", "Toggled GUI", MB_ICONINFORMATION, false);
			//hideGui = !hideGui;
		}

		//switch (currentRSState)
		//{
		//case RSState::Wireframe:
		//	graphics->m_deviceContext->RSSetState(graphics->wireframeRasterizerState.Get());
		//	break;
		//default:
		//	graphics->m_deviceContext->RSSetState(nullptr);
		//	break;
		//}

		if (isGameSceneActive) {
			gameFrame->Bind();
			gameFrame->ClearBuffer(0u, { 0.0f, 0.0f, 0.0f });
			gameFrame->ClearDepthStencil();

			SceneRegistry::OnRuntimeRender();

			gameFrame->Unbind();
		}

		ScenePanel::Render(*SceneRegistry::get_current_scene().get());

		OnGUI();
	}

	void HeliosEditor::OnGUI()
	{
		//static ImGuiIO& io = ImGui::GetIO(); (void)io;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		static bool dock_open = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Dockspace Demon", &dock_open, window_flags);
		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		if (ImGui::IsKeyReleased(ImGuiKey_S) && ImGui::IsKeyReleased(ImGuiKey_ModCtrl))
			Project::SaveScene(ImGui::IsKeyReleased(ImGuiKey_LeftShift) || ImGui::IsKeyReleased(ImGuiKey_RightShift));

		DrawToolbar(currentMode, panels);

		for (auto& panel : panels)
		{
			if (panel->m_window_open)
			{
				std::string tmp = panel->GetName();
				if(panel->IsCustomBegin())
				{
					HL_PROFILE_BEGIN(("GUI - " + tmp));
					panel->OnUpdate();
					HL_PROFILE_END();
				} else
				{
					if (ImGui::Begin(panel->GetName().c_str(), &panel->m_window_open)) {
						HL_PROFILE_BEGIN(("GUI - " + tmp));
						panel->OnUpdate();
						HL_PROFILE_END();
					}
					ImGui::End();
				}
			}
		}

		auto i = std::begin(panels);

		while (i != std::end(panels)) {
			if ((*i)->destroy_window)
				i = panels.erase(i);
			else
				++i;
		}

		AssetRegistry::ShowRegistryWindow();
		AssetRegistry::ShowTextureSelect();
		

		if (show_profiler_window)
		{
			if (ImGui::Begin("Profiler", &show_profiler_window))
			{
				HL_PROFILE_BEGIN("GUI - Profiler");
				bool isProfilingThisFrame = Profiler::isProfiling();

				if (ImGui::Checkbox("Profiler Enabled", &isProfilingThisFrame))
				{
					HL_PROFILE_TOGGLE_PROFILING();
				}
				ImGui::SameLine();

				static bool slow_mode = true;
				static float update_interval = 3.0f;
				static Profiler::FrameProfile selectedFrameProfile(0);

				if (ImGui::Button("Clear")) { Profiler::clear(); selectedFrameProfile = (0); }

				if (Profiler::isProfiling())
				{
					if (slow_mode)
					{
						static float tmr = update_interval;
						tmr += Time::deltaTime();

						if (tmr >= update_interval)
						{
							if (Profiler::getResults().size() > 1) selectedFrameProfile = Profiler::getResults()[Profiler::getResults().size() - 2];
							tmr = 0.0f;
						}
					}
					else if (Profiler::getResults().size() > 1) selectedFrameProfile = Profiler::getResults()[Profiler::getResults().size() - 2];
				}



				if (selectedFrameProfile.frameTime)
				{
					uint32_t selectedFrame = 0u;
					unsigned long long frametime = selectedFrameProfile.frameTime / 1000.0f;

					ImGui::Text((std::to_string(Profiler::getResults().size() - 1) + " Frames!").c_str());
					ImGui::Text(("Last frame time: " + std::to_string(selectedFrameProfile.frameTime / 1000.0f) + "ms.").c_str());

					ImGui::PlotLines("Frametime", [](void* data, int idx) { return (Profiler::getResults()[min(max(Profiler::getResults().size() - 300, 0) + idx, Profiler::getResults().size() - 1)].frameTime / 1000.0f); }, nullptr, min(Profiler::getResults().size() - 1, 299), 0, 0, FLT_MAX, FLT_MAX, ImVec2(1800, 150));


					ImGui::Checkbox("Slow Mode", &slow_mode);
					ImGui::SameLine();
					ImGui::DragFloat("Update Interval", &update_interval);

					HL_PROFILE_BEGIN("GUI - Profiler DRAW");
					ProfilerTimeline::Draw(selectedFrameProfile);
					HL_PROFILE_END();

					/*for (auto x : selectedFrameProfile.results)
					{
						ImGui::Button(x.Name, ImVec2(0, 0));
					}*/
				}

				HL_PROFILE_END();
			}
			ImGui::End();
		}

		HL_PROFILE_BEGIN("GUI - Project Explorer");
		ProjectExplorerWindow(Project::GetAssetsPath());
		HL_PROFILE_END();

		ImGui::End();
	}

}