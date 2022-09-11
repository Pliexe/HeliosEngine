#define HELIOS_EDITOR

#include <iostream>
#include <windows.h>

#include <Helios.h>
#include <Helios/Resources/EngineTexture.h>
#include <Helios/Resources/Color.h>
#include <Helios/Renderer/Renderer2D.h>

#include <Helios/Utils/ErrorHandling.h>

#include <d2d1_3.h>

#include "Helios/Utils/ErrorHandling.h"
#include <filesystem>

#include <Panels/ProjectExplorer.h>
#include <ProjectManager.h>

#include "Icons.h"
#include "Panels/BasePanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"

#include "HeliosEditor_Macros.h"
#include <imgui_impl_win32.cpp>
#include <imgui_impl_dx11.cpp>
#include "Helios/Translation/Matrix.h"
#include <sstream>

static std::filesystem::path currentScene;
StartupConfig startupConfig;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static Helios::Components::Transform cameraTransform;
static Helios::Components::Camera cameraPropeties;

using namespace Helios;

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

namespace Helios {
	class GameEngine : public Helios::Application {
	private:

		InspectorPanel inspector;
		HierarchyPanel hierarchy;

		std::vector<Editor::IPanel*> panels;

		/*GameObject* InspectorCreateElementsList(Transform2D* parent = nullptr) {
			if (ImGui::MenuItem("Create Empty")) {


				return &GameObject::InstantiateObject("Empty GameObject", parent);
			}
			else return nullptr;
		}*/

		//bool InspectorElement(GameObject* o, GameObject** selected, int count, float width, float indent = 40) {
		//	if (o->transform.parent && indent == 40) return false;
		//	bool clicked = false;
		//	float height = 50;
		//	ImGui::PushID("HierarchyElement");
		//	ImGui::PushID(indent);
		//	ImGui::PushID(count);

		//	ImGui::PushStyleColor(ImGuiCol_Header, o == *selected ? IM_COL32(255, 255, 255, 25) : IM_COL32(0, 0, 0, 0));
		//	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
		//	//ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 255, 255, 100));

		//	static char buf[100];
		//	strcpy_s(buf, o->GetName().c_str());


		//	//ImGui::Text(o->m_name.c_str());



		//	//ImGui::Indent();

		//	static int rename = -1;

		//	ImGui::BeginGroup();
		//	bool is_open;
		//	ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
		//	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 0));
		//	is_open = ImGui::TreeNodeEx("##t_gm", o->transform.children.size() > 0 ? ImGuiTreeNodeFlags_Selected : (ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Leaf));
		//	ImGui::PopStyleColor(2);

		//	//if (rename != count)
		//	//{
		//	//	ImGui::SameLine();

		//	//	if (ImGui::Selectable("##s_gm", o == *selected, ImGuiSelectableFlags_AllowItemOverlap)) {
		//	//		/*if(ImGui::IsItemClicked() && o == *selected)
		//	//			rename = count;*/

		//	//		(*selected) = o;
		//	//	}
		//	//}


		//	ImGui::SameLine();
		//	static bool firstTimeActive = true;
		//	if (rename == count)
		//	{
		//		char tmp[100];
		//		strcpy_s(tmp, o->m_name.c_str());
		//		ImGui::SetCursorPosX(indent);
		//		if (firstTimeActive) {
		//			ImGui::SetKeyboardFocusHere();
		//			firstTimeActive = false;
		//		}
		//		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 3.5f));
		//		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2);
		//		ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 255));
		//		if (ImGui::InputText("##editGameobjectName", tmp, 100, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::IsItemDeactivatedAfterEdit())
		//		{
		//			if (tmp[0] != '\0') o->m_name = tmp;
		//			rename = -1;
		//			firstTimeActive = true;
		//		}
		//		else if (ImGui::IsItemDeactivated()) {
		//			rename = -1;
		//			firstTimeActive = true;
		//		}
		//		ImGui::PopStyleColor();
		//		ImGui::PopStyleVar(2);
		//	}
		//	else
		//	{
		//		ImGui::SetCursorPosX(0);
		//		if(ImGui::Selectable("##obj_select", *selected == o, 0, ImVec2(width, 20))) {
		//			(*selected) = o;
		//			inspector.SetSelected<InspectorPanel::SelectedType::GameObject>(o);
		//		}

		//		ImGui::SameLine();

		//		static bool selectedCount = -1;
		//		if (*selected == o) selectedCount = count;
		//		if (ImGui::IsKeyReleased(ImGuiKey_F2))
		//			rename = selectedCount;
		//		else {
		//			ImGui::OpenPopupOnItemClick("ObjectContext", ImGuiPopupFlags_MouseButtonRight);
		//			if (ImGui::BeginPopup("ObjectContext"))
		//			{
		//				clicked = true;
		//				if (ImGui::MenuItem("Rename")) {
		//					rename = count;
		//				}
		//				//InspectorCreateElementsList(&o->transform);
		//				ImGui::Separator();
		//				ImGui::EndPopup();
		//			}
		//		}

		//		ImGui::SetCursorPos(ImVec2(indent, ImGui::GetCursorPosY() + 2));
		//		ImGui::Text(o->m_name.c_str());
		//	}

		//	if (is_open) {
		//		if (o->transform.children.size() > 0)
		//		{
		//			int i = 0;
		//			for (auto& t : o->transform.children)
		//			{
		//				i++;
		//				InspectorElement(t->gameObject, selected, i, width, indent + 10.0f);
		//			}
		//		}

		//		ImGui::TreePop();
		//	}

		//	ImGui::EndGroup();

		//	ImGui::PopStyleColor(2);
		//	ImGui::PopID();
		//	ImGui::PopID();
		//	ImGui::PopID();
		//	return clicked;
		//}

		bool inPlayMode = false;
		
		LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
		{
			if (ImGui_ImplWin32_WndProcHandler(m_hWnd, uMsg, wParam, lParam))
				return true;
			if (uMsg == WM_DPICHANGED) {
				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
				{
					//const int dpi = HIWORD(wParam);
					//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
					const RECT* suggested_rect = (RECT*)lParam;
					::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
			else Application::WindowProc(uMsg, wParam, lParam);
		}

	public:

		static const wchar_t* WindowClassName() { return L"Helios Game Window"; };

		GameEngine()
		{
			panels.push_back(&inspector);
			panels.push_back(&hierarchy);

			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
			//io.ConfigViewportsNoAutoMerge = true;
			//io.ConfigViewportsNoTaskBarIcon = true;

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			io.FontDefault = io.Fonts->AddFontFromFileTTF("fonts/NotoSans/NotoSans-Regular.ttf", 17.0f);

			auto add_col = [](ImVec4 col, float offset) {
				return ImVec4 { col.x + offset, col.y + offset, col.z + offset, 1.0f };
			};

			auto& colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.27f, 1.0f);
			
			// Headers
			colors[ImGuiCol_Header] = { 0.141f, 0.149f, 0.145f, 1.0f };
			colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
			colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

			// Buttons
			colors[ImGuiCol_Button] = { 0.078f, 0.078f, 0.078f, 1.0f };
			colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
			colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

			// Frame BG
			colors[ImGuiCol_FrameBg] = { 0.133f, 0.141f, 0.137f, 1.0f };
			colors[ImGuiCol_FrameBgHovered] = add_col(colors[ImGuiCol_FrameBg], 0.1f);
			colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

			// Tabs
			colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
			colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
			colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

			// Title
			colors[ImGuiCol_TitleBg] = { 0.105f, 0.105f, 0.109f, 1.0f };
			colors[ImGuiCol_TitleBgActive] = { 0.212f, 0.208f, 0.220f, 1.0f };
			colors[ImGuiCol_TitleBgCollapsed] = { 0.212f, 0.208f, 0.220f, 1.0f };
			
		}

		// Inherited via Application
		virtual void Init() override
		{
						
			try {
				ICON_FOLDER = std::make_shared<EngineTexture>(EngineTexture("resources/folder-icon.png"));
				ICON_FOLDER_EMPTY = std::make_shared<EngineTexture>(EngineTexture("resources/folder-icon-empty.png"));
				ICON_FILE_UNKNOWN = std::make_shared<EngineTexture>(EngineTexture("resources/unknown_file.png"));

				ICON_FILE_TXT = std::make_shared<EngineTexture>(EngineTexture("resources/text_file.png"));
				ICON_FILE_C = std::make_shared<EngineTexture>(EngineTexture("resources/c_file.png"));
				ICON_FILE_H = std::make_shared<EngineTexture>(EngineTexture("resources/h_file.png"));
				ICON_FILE_CPP = std::make_shared<EngineTexture>(EngineTexture("resources/cpp_file.png"));
				ICON_FILE_HPP = std::make_shared<EngineTexture>(EngineTexture("resources/hpp_file.png"));
				ICON_FILE_FONT = std::make_shared<EngineTexture>(EngineTexture("resources/ttf_file.png"));
				ICON_FILE_SCENE = std::make_shared<EngineTexture>(EngineTexture("resources/scene-file.png"));
				ICON_FILE_IMAGE = std::make_shared<EngineTexture>(EngineTexture("resources/image_file.png"));
			}
			catch (std::runtime_error e) {
				ShowMessage("Error loading icons!", e.what(), MB_ICONERROR, true);
				exit(-100);
			}

			
			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(m_hWnd);
			ImGui_ImplDX11_Init(graphics->m_device, graphics->m_deviceContext);

			using namespace Helios;

			//Project::TryLoad();

			Helios::SceneManager::AddScene("Test", [](Helios::Scene& scene) {
				Helios::GameObject::CreateMainCamera();
			});

			SceneManager::LoadScene("Test");
		}

		void OnGUI() {
			/*static bool demo_window = true;
			if (demo_window)
				ImGui::ShowDemoWindow(&demo_window);*/
			
			static ImGuiIO& io = ImGui::GetIO();
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

			static GameObject* selected = nullptr;

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Scene", "", false, !this->inPlayMode))
					{
						selected = nullptr;
						Project::LoadSampleScene();
					}
					if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, !this->inPlayMode))
					{
						selected = nullptr;
						Project::OpenSceneDialog();
					}


					ImGui::Separator();

					if (ImGui::MenuItem("Save", "Ctrl+S", false, !this->inPlayMode))
						Project::SaveScene();
					
					if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, !this->inPlayMode))
						Project::SaveScene(true);

					ImGui::Separator();

					if (ImGui::MenuItem("New Project"))
					{
						ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
					}

					if (ImGui::MenuItem("Open Project"))
					{
						ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Exit")) {
						Application::Quit();
					}
					ImGui::EndMenu();
				}

				if (ImGui::Button("Play")) {
						this->inPlayMode != this->inPlayMode;
				}
				
				ImGui::EndMenuBar();
			}

			ImGui::ShowDemoWindow();

			for (auto& panel : panels)
			{
				bool is_open = true;
				std::string tmp = panel->GetName();
				if (ImGui::Begin(panel->GetName().c_str(), &is_open)) {
					panel->OnUpdate();
				}
				ImGui::End();
			}

			if (ImGui::Begin("Game View 2"))
			{
				if (graphics->pTextureView)
					ImGui::Image((void*)graphics->pTextureViewtest, ImVec2(500, 500));
			}
			ImGui::End();


			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
			if (ImGui::Begin("Game"))
			{
				if (graphics->pTextureView)
				{
					ImVec2 viewportSize = ImGui::GetContentRegionAvail();
					auto currentViewportSize = Renderer2D::GetRenderTargetSize();
					if (viewportSize.x != currentViewportSize.x|| viewportSize.y != currentViewportSize.y)
					{
						Renderer2D::Resize(viewportSize.x, viewportSize.y);
						WindowCordinates::SetSize(viewportSize.x, viewportSize.y);
					}
					//ImGui::Image((void*)graphics->pTextureView, ImVec2(viewportSize));
					ImGui::Image(Renderer2D::GetImGuiTexture(), ImVec2(viewportSize));
				}

				static bool notf = true;

				if (!SceneManager::GetCurrentScene().lock()->IsPrimaryCameraSet()) {
					if (notf) {
						auto pos = ImGui::GetWindowPos();
						auto size = ImGui::GetWindowSize();
						ImGui::SetNextWindowFocus();
						ImGui::SetNextWindowBgAlpha(1.0f);
						ImGui::SetWindowFontScale(2);
						ImGui::SetNextWindowPos(ImVec2(pos.x + (size.x / 2.0f) - 200.0f, pos.y + (size.y / 2.0f) - 100.0f));
						ImGui::BeginChild("MissingCameraText", ImVec2(400, 200), true);
						//ImGui::Button("Test");
						//ImGui::SetCursorPos(ImVec2(0, 0));
						ImGui::Text("No Camera Present for rendering!");
						if (ImGui::Button("Close")) {
							notf = false;
						}
						ImGui::EndChild();
					}
				}
				else notf = true;

				if (ImGui::IsWindowHovered())
				{
					static ImVec2 origin = ImGui::GetMousePos();

					if(ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
						ImVec2 current = ImGui::GetMousePos();

						auto difference = Vector2(origin.x, origin.y) - Vector2(current.x, current.y);
						auto normalizedCordinates = (Vector2(origin.x, origin.y) - Vector2(current.x, current.y)).normalize();

						//cameraTransform.rotation += (Vector2(-origin.y, origin.x) - Vector2(-current.y, current.x)).normalize() * Time::deltaTime() * 100.0f;
						
						std::cout << "Diffrence: x: " << difference.x << ", y: " << difference.y << std::endl;
						std::cout << "Normal: x: " << normalizedCordinates.x << ", y: " << normalizedCordinates.y << std::endl;



						if(ImGui::IsKeyDown(ImGuiKey_W))
							cameraTransform.position += Vector3::up() * Time::deltaTime() * 10.0f;

						
						//origin = ImGui::GetMousePos();
						SetCursorPos(origin.x, origin.y);
					}
					else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					}
					else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
						origin = ImGui::GetMousePos();
						std::cout << "Start" << std::endl;
					}
				}

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R)) {
					cameraTransform.position = Vector3::zero();
					cameraTransform.rotation = Quanterion::zero();
				}
				
			}
			ImGui::End();
			ImGui::PopStyleVar();


			
			

			ProjectExplorerWindow(Project::GetAssetsPath());

			ImGui::End();
		}

		void OnUpdate() override {
			if (this->inPlayMode) Application::OnUpdate();
		}

		void OnRender() override {
			static auto& io = ImGui::GetIO();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Vector3 rot;

			rot.x = 0.9f;
			rot.y = -1.0;
			rot.z = -0.3f;

			Quanterion q = Quanterion::EulerRads(rot);

			std::stringstream str;

			str << "Euler: " << "x: " << rot.x << ", y: " << rot.y << ", z: " << rot.z << std::endl << std::endl;

			str << "Quanterion: " << "x: " << q.x << ", y: " << q.y << ", z: " << q.z << ", w: " << q.w << std::endl << std::endl;

			Vector3 euler = q.euler();

			str << "Converted Euler: " << "x: " << euler.x << ", y: " << euler.y << ", z: " << euler.z << std::endl << std::endl;

			ShowMessage("Quanterion", str.str());

			OnGUI();

			ImGui::EndFrame();
			ImGui::Render();
			
			graphics->m_deviceContext->OMSetRenderTargets(2, &graphics->m_mainRenderTarget, NULL);
			graphics->ClearRenderTarget(0.5f, 0.0f, 1.0f);

#pragma region Rendering
			if (SceneManager::currentScene) {
				//graphics->m_renderTarget2D->BeginDraw();

				//SceneManager::currentScene->OnUpdateRuntime();
				SceneManager::currentScene->OnUpdateEditor(cameraTransform, cameraPropeties);

				//SceneManager::Render(*graphics);

				//Render(*graphics);

				//graphics->m_renderTarget2D->EndDraw();
			}
#pragma endregion

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			graphics->EndFrame();

		}
		
		~GameEngine() {
			ImGui::DestroyContext();
		}

	};
}

Helios::Application* Helios::CreateApplication() { return new Helios::GameEngine(); }