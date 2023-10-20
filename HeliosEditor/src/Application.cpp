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
#include <Shlwapi.h>

#include "Helios/Math/Matrix.h"
#include <Helios/Math/Quaternion.h>
#include <sstream>
#include <Helios/Graphics/Framebuffer.h>

#include "AssetRegistry.h"
#include <Helios/Resources/Shader.h>
#include <Helios/Input/InputManager.h>
#include <Helios/Input/KeyCodes.h>

#include "ApplicationStyle.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Graphics/GizmosRenderer.h"

#include "Helios/Scene/EditorCamera.h"

#include "DirectXMath.h"
#include "InitWindow.h"
#include "GUI/Toolbar.h"
#include "Helios/Core/UUID.h"
#include "Helios/Resources/Material.h"
#include "Panels/ExplorerPanel.h"
#include "Panels/ProfilerPanel.h"
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
		/*Project::AttemptLoad();
		SetCurrentPath(Project::GetAssetsPath());*/
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
	#ifdef HELIOS_PLATFORM_WINDOWS
		specs.graphicsAPI = Graphics::API::Direct3D11;
	#else
		specs.graphicsAPI = Graphics::API::OpenGL;
	#endif
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

		InitWindow* initWindow = InitWindow::Create();
		initWindow->SetTextA("Loading Engine...");

		try
		{
			constexpr static float to_load_count = 22.f;

			initWindow->SetB("Loading icon. 1 out of " + std::to_string(to_load_count), 1.0f / to_load_count);
			ICON_FOLDER_EMPTY = Texture2D::Create("Resources/Icons/folder_empty.png");
			initWindow->SetB("Loading icon. 2 out of " + std::to_string(to_load_count), 2.0f / to_load_count);
			ICON_FILE_UNKNOWN = Texture2D::Create("Resources/Icons/unknown_file.png");
			initWindow->SetB("Loading icon. 3 out of " + std::to_string(to_load_count), 3.0f / to_load_count);
			ICON_FILE_TXT = Texture2D::Create("Resources/Icons/text_file.png");
			initWindow->SetB("Loading icon. 4 out of " + std::to_string(to_load_count), 4.0f / to_load_count);
			ICON_FILE_C = Texture2D::Create("Resources/Icons/c_file.png");
			initWindow->SetB("Loading icon. 5 out of " + std::to_string(to_load_count), 5.0f / to_load_count);
			ICON_FILE_H = Texture2D::Create("Resources/Icons/h_file.png");
			initWindow->SetB("Loading icon. 6 out of " + std::to_string(to_load_count), 6.0f / to_load_count);
			ICON_FILE_CPP = Texture2D::Create("Resources/Icons/cpp_file.png");
			initWindow->SetB("Loading icon. 7 out of " + std::to_string(to_load_count), 7.0f / to_load_count);
			ICON_FILE_HPP = Texture2D::Create("Resources/Icons/hpp_file.png");
			initWindow->SetB("Loading icon. 8 out of " + std::to_string(to_load_count), 8.0f / to_load_count);
			ICON_FILE_FONT = Texture2D::Create("Resources/Icons/ttf_file.png");
			initWindow->SetB("Loading icon. 9 out of " + std::to_string(to_load_count), 9.0f / to_load_count);
			ICON_FILE_SCENE = Texture2D::Create("Resources/Icons/scene_file.png");
			initWindow->SetB("Loading icon. 10 out of " + std::to_string(to_load_count), 10.0f / to_load_count);
			ICON_FILE_IMAGE = Texture2D::Create("Resources/Icons/image_file.png");
			initWindow->SetB("Loading icon. 11 out of " + std::to_string(to_load_count), 11.0f / to_load_count);
			ICON_PLAY_WHITE = Texture2D::Create("Resources/Icons/play_white.png");
			initWindow->SetB("Loading icon. 12 out of " + std::to_string(to_load_count), 12.0f / to_load_count);
			ICON_PAUSE_WHITE = Texture2D::Create("Resources/Icons/pause_white.png");
			initWindow->SetB("Loading icon. 13 out of " + std::to_string(to_load_count), 13.0f / to_load_count);
			ICON_STOP_WHITE = Texture2D::Create("Resources/Icons/stop_white.png");
			initWindow->SetB("Loading icon. 14 out of " + std::to_string(to_load_count), 14.0f / to_load_count);
			ICON_FOLDER = Texture2D::Create("Resources/Icons/folder.png");

			initWindow->SetB("Loading icon. 15 out of " + std::to_string(to_load_count), 15.0f / to_load_count);
			ICON_UI_GROUP_TILES_WHITE = Texture2D::Create("Resources/ui_explorer_group_tiles_white.png");
			initWindow->SetB("Loading icon. 16 out of " + std::to_string(to_load_count), 16.0f / to_load_count);
			ICON_UI_GROUP_LIST_WHITE = Texture2D::Create("Resources/ui_explorer_group_list_white.png");
			initWindow->SetB("Loading icon. 17 out of " + std::to_string(to_load_count), 17.0f / to_load_count);
			ICON_UI_GROUP_ICONS_WHITE = Texture2D::Create("Resources/ui_explorer_group_icons_white.png");

			initWindow->SetB("Loading icon. 18 out of " + std::to_string(to_load_count), 18.0f / to_load_count);
			ICON_UI_GROUP_TILES_BLACK = Texture2D::Create("Resources/ui_explorer_group_tiles_black.png");
			initWindow->SetB("Loading icon. 19 out of " + std::to_string(to_load_count), 19.0f / to_load_count);
			ICON_UI_GROUP_LIST_WHITE = Texture2D::Create("Resources/ui_explorer_group_list_black.png");
			initWindow->SetB("Loading icon. 20 out of " + std::to_string(to_load_count), 20.0f / to_load_count);
			ICON_UI_GROUP_ICONS_WHITE = Texture2D::Create("Resources/ui_explorer_group_icons_black.png");

			initWindow->SetB("Loading icon. 21 out of " + std::to_string(to_load_count), 21.0f / to_load_count);
			ICON_UI_REFRESH_WHITE = Texture2D::Create("Resources/ui_refresh_white.png");
			initWindow->SetB("Loading icon. 22 out of " + std::to_string(to_load_count), 22.0f / to_load_count);
			ICON_UI_REFRESH_BLACK = Texture2D::Create("Resources/ui_refresh_black.png");

		} catch (HeliosException e)
		{
			MessageBoxA(nullptr, "Error loading icons!", std::to_string(e.what()).c_str(), MB_ICONERROR);
			exit(-100);
		}

		Project::LoadAssets();

		if(ICON_FOLDER_EMPTY == nullptr)
			MessageBoxA(nullptr, "Error loading icons!", "ICON_FOLDER_EMPTY == nullptr", MB_ICONERROR);
		
		gameFrame = Framebuffer::Create(300, 300, {
            Framebuffer::Format::R8G8B8A8_UNORM,
            //Framebuffer::Format::R32F,
            Framebuffer::Format::D32F
        });		

		
		initWindow->Quit();

		AssetRegistry::Init();

		using namespace Helios;

		Project::TryLoad();

		// tmp

		SceneRegistry::LoadEmptyScene("New Scene");
		auto ent = SceneRegistry::m_activeScenes[0]->InstantiateObject(Vector3{ 0, 0, 0 });
		//auto ent = SceneRegistry::m_activeScenes[0]->InstantiateObject(Vector3{ 0, 0, -5 });
		auto& mrc = ent.AddScopedComponent<MeshRendererComponent>();
		mrc.mesh = Mesh::GenerateCube();
		mrc.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Clamp);
		auto ent2 = SceneRegistry::m_activeScenes[0]->InstantiateObject(Vector3{ 0, 0, 0 });
		ent2.AddScopedComponent<DirectionalLightComponent>(Color::White, 1.0f);
		Transform(ent2).SetLocalRotation(Quaternion::FromEuler(-45, 0, 0));

		panels.push_back(&inspector);
		panels.push_back(&hierarchy);
		panels.push_back(&gameViewPanel);
		panels.push_back(&scenePanel);
		panels.push_back(new ProfilerPanel());
		panels.push_back(new Editor::ExplorerPanel());
		//panels.push_back(&AssetRegistry::Get());

		UUID uuid = UUID();
		UUID uuid2 = UUID();
		UUID uuid3 = UUID(uuid);

		std::cout << uuid.toString() << std::endl;
		std::cout << uuid2.toString() << std::endl;
		std::cout << uuid3.toString() << std::endl;

		std::cout << uuid.toString() << " == " << uuid2.toString() << " = " << (uuid == uuid2) << std::endl;
		std::cout << uuid.toString() << " == " << uuid3.toString() << " = " << (uuid == uuid3) << std::endl;

		uint8_t data[16];
		std::memcpy(data, (uint8_t*)uuid, 16);

		// print whole hex
		std::cout << std::hex;

		for (int i = 0; i < 16; i++)
		{
			std::cout << (int)data[i] << " ";
		}

		UUID uuid4 = UUID::fromString(uuid.toString());
		std::cout << std::endl << uuid4.toString() << std::endl;
		std::cout << uuid4.toString() << " == " << uuid.toString() << " = " << (uuid4 == uuid) << std::endl;

		std::cout << std::endl;

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

		//if (InputManager::IsKeyPressed(HL_KEY_CONTROL) && InputManager::IsKeyPressed(HL_KEY_F10))
		//{
		//	DepricatedApplication::ShowMessage("INFO!", "Toggled GUI", MB_ICONINFORMATION, false);
		//	//hideGui = !hideGui;
		//}

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

		//ScenePanel::Render(*SceneRegistry::get_current_scene().get());

		if (InputManager::IsKeyPressed(HL_KEY_CONTROL) && InputManager::IsKeyPressed(HL_KEY_F10))
		{
			MessageBoxA(nullptr, "Toggled GUI", "INFO!", MB_ICONINFORMATION);
			m_barebones = !m_barebones;
		}

		if (!m_barebones) 
			OnGUI();
	}

	void HeliosEditor::OnRender()
	{
		if (m_barebones)
		{
			GetWindow()->GetContext().BindDefaultFramebuffer();
			static EditorCamera camera;
			camera.SetViewportSize({ GetWindow()->GetWidth(), GetWindow()->GetHeight() });
			SceneRegistry::OnEditorRender(camera);
		}
		else
		{

			for (auto& panel : panels)
			{
				ScenePanel* tmp;

				if ((tmp = dynamic_cast<ScenePanel*>(panel)) == nullptr) continue;

				tmp->RenderFramebuffer();
			}
		}
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

		static bool showStyleEditor = true;

		/*if (showStyleEditor)
		{
			ImGui::Begin("Style Editor", &showStyleEditor);
			ImGui::ShowStyleEditor();
			ImGui::End();
		}*/

		ImGui::Begin("Colors");

		auto& colors = ImGui::GetStyle().Colors;
		ImGui::ColorEdit4("WindowBg", &colors[ImGuiCol_WindowBg].x);

		ImGui::ColorEdit4 ("FrameBg", &colors[ImGuiCol_FrameBg].x);
		ImGui::ColorEdit4 ("FrameBgHovered", &colors[ImGuiCol_FrameBgHovered].x);
		ImGui::ColorEdit4 ("FrameBgActive", &colors[ImGuiCol_FrameBgActive].x);
		ImGui::ColorEdit4 ("TitleBg", &colors[ImGuiCol_TitleBg].x);
		ImGui::ColorEdit4 ("TitleBgActive", &colors[ImGuiCol_TitleBgActive].x);
		ImGui::ColorEdit4 ("TitleBgCollapsed", &colors[ImGuiCol_TitleBgCollapsed].x);
		ImGui::ColorEdit4 ("MenuBarBg", &colors[ImGuiCol_MenuBarBg].x);
		ImGui::ColorEdit4 ("ScrollbarBg", &colors[ImGuiCol_ScrollbarBg].x);
		ImGui::ColorEdit4 ("ScrollbarGrab", &colors[ImGuiCol_ScrollbarGrab].x);
		ImGui::ColorEdit4 ("ScrollbarGrabHovered", &colors[ImGuiCol_ScrollbarGrabHovered].x);
		ImGui::ColorEdit4 ("ScrollbarGrabActive", &colors[ImGuiCol_ScrollbarGrabActive].x);
		ImGui::ColorEdit4 ("CheckMark", &colors[ImGuiCol_CheckMark].x);
		ImGui::ColorEdit4 ("SliderGrab", &colors[ImGuiCol_SliderGrab].x);
		ImGui::ColorEdit4 ("SliderGrabActive", &colors[ImGuiCol_SliderGrabActive].x);
		ImGui::ColorEdit4 ("Button", &colors[ImGuiCol_Button].x);
		ImGui::ColorEdit4 ("ButtonHovered", &colors[ImGuiCol_ButtonHovered].x);
		ImGui::ColorEdit4 ("ButtonActive", &colors[ImGuiCol_ButtonActive].x);
		ImGui::ColorEdit4 ("Header", &colors[ImGuiCol_Header].x);
		ImGui::ColorEdit4 ("HeaderHovered", &colors[ImGuiCol_HeaderHovered].x);
		ImGui::ColorEdit4 ("HeaderActive", &colors[ImGuiCol_HeaderActive].x);
		ImGui::ColorEdit4 ("Separator", &colors[ImGuiCol_Separator].x);
		ImGui::ColorEdit4 ("SeparatorHovered", &colors[ImGuiCol_SeparatorHovered].x);
		ImGui::ColorEdit4 ("SeparatorActive", &colors[ImGuiCol_SeparatorActive].x);
		ImGui::ColorEdit4 ("ResizeGrip", &colors[ImGuiCol_ResizeGrip].x);
		ImGui::ColorEdit4 ("ResizeGripHovered", &colors[ImGuiCol_ResizeGripHovered].x);
		ImGui::ColorEdit4 ("ResizeGripActive", &colors[ImGuiCol_ResizeGripActive].x);
		ImGui::ColorEdit4 ("Tab", &colors[ImGuiCol_Tab].x);
		ImGui::ColorEdit4 ("TabHovered", &colors[ImGuiCol_TabHovered].x);
		ImGui::ColorEdit4 ("TabActive", &colors[ImGuiCol_TabActive].x);
		ImGui::ColorEdit4 ("TabUnfocused", &colors[ImGuiCol_TabUnfocused].x);
		ImGui::ColorEdit4 ("TabUnfocusedActive", &colors[ImGuiCol_TabUnfocusedActive].x);
		
		ImGui::End();

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
		

		HL_PROFILE_BEGIN("GUI - Project Explorer");
		//ProjectExplorerWindow(Project::GetAssetsPath());
		HL_PROFILE_END();

		ImGui::End();
	}

}