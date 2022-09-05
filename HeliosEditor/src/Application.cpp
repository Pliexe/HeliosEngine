#define HELIOS_EDITOR

#include <iostream>
#include <windows.h>

#include <Helios.h>
#include <Helios/Application.h>
#include <Helios/Time.h>
#include <Helios/GameObjects/GameObject.h>
#include <Helios/GameObjects/Components/RectangleComponent.h>
#include <Helios/GameObjects/Components/EllipseComponent.h>
#include <Helios/Resources/Brushes/SolidBrush.h>
#include <Helios/Resources/Color.h>
#include <Helios/GameObjects/Components/Camera.h>

#include <Helios/Utils/ErrorHandling.h>

#include <Helios/Resources/EngineTexture.h>

#include <d2d1_3.h>

//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx10.h>

#include "Components/CameraInspector.h"
#include "Helios/Utils/ErrorHandling.h"
#include <filesystem>

#include <Panels/ProjectExplorer.h>
#include <ProjectManager.h>

#include "Icons.h"

#include "HeliosEditor_Macros.h"
#include <imgui_impl_win32.cpp>
#include <imgui_impl_dx11.cpp>

static std::filesystem::path projectPath;
StartupConfig startupConfig;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Helios;

int ValidateInit() {
	
	if (__argc > 1) {

		int argCount;
		LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argCount);
		std::filesystem::path selected_path(args[1]);
		LocalFree(args);

		if (!std::filesystem::is_directory(selected_path)) {
			MessageBoxA(NULL, "The path given is invalid!", "Failed to open project!", MB_ICONERROR);
			return HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
		}
		
		int code;
		if ((code = Helios::Project::IsValidProject(selected_path.string().c_str())) < 0) {
			std::cout << "Given directory: \"" << selected_path << "\" is not a valid project! Code: " << code << std::endl;
			if (MessageBoxA(NULL, "The directory given is not a valid project! Do you want to create a new project in this directory", "Failed to open project!", MB_RETRYCANCEL | MB_ICONERROR) != IDRETRY) 
				return HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
		}

		startupConfig = Project::DeserializeStartupConfig((selected_path / "Settings" / "Startup.config").string().c_str());
	}
	else {
		ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
		return HELIOS_EXIT_CODE_INVALID_ARGS;
	}

	return 0;
}

namespace Helios {
	class GameEngine : public Helios::Application {
	private:

		GameObject* InspectorCreateElementsList(Transform* parent = nullptr) {
			if (ImGui::MenuItem("Create Empty")) {
				return &GameObject::InstantiateObject("Empty GameObject", parent);
			}
			else return nullptr;
		}

		bool InspectorElement(GameObject* o, GameObject** selected, int count, float width, float indent = 40) {
			if (o->transform.parent && indent == 40) return false;
			bool clicked = false;
			float height = 50;
			ImGui::PushID("HierarchyElement");
			ImGui::PushID(indent);
			ImGui::PushID(count);

			ImGui::PushStyleColor(ImGuiCol_Header, o == *selected ? IM_COL32(255, 255, 255, 25) : IM_COL32(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(255, 255, 255, 100));

			static char buf[100];
			strcpy_s(buf, o->m_name.c_str());


			//ImGui::Text(o->m_name.c_str());



			//ImGui::Indent();

			static int rename = -1;

			ImGui::BeginGroup();
			bool is_open;
			ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 0));
			is_open = ImGui::TreeNodeEx("##t_gm", o->transform.children.size() > 0 ? ImGuiTreeNodeFlags_Selected : (ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Leaf));
			ImGui::PopStyleColor(2);

			//if (rename != count)
			//{
			//	ImGui::SameLine();

			//	if (ImGui::Selectable("##s_gm", o == *selected, ImGuiSelectableFlags_AllowItemOverlap)) {
			//		/*if(ImGui::IsItemClicked() && o == *selected)
			//			rename = count;*/

			//		(*selected) = o;
			//	}
			//}


			ImGui::SameLine();
			static bool firstTimeActive = true;
			if (rename == count)
			{
				char tmp[100];
				strcpy_s(tmp, o->m_name.c_str());
				ImGui::SetCursorPosX(indent);
				if (firstTimeActive) {
					ImGui::SetKeyboardFocusHere();
					firstTimeActive = false;
				}
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 3.5f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2);
				ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 255, 255, 255));
				if (ImGui::InputText("##editGameobjectName", tmp, 100, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::IsItemDeactivatedAfterEdit())
				{
					if (tmp[0] != '\0') o->m_name = tmp;
					rename = -1;
					firstTimeActive = true;
				}
				else if (ImGui::IsItemDeactivated()) {
					rename = -1;
					firstTimeActive = true;
				}
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);
			}
			else
			{
				ImGui::SetCursorPosX(0);
				if(ImGui::Selectable("##obj_select", *selected == o, 0, ImVec2(width, 20))) {
					(*selected) = o;
				}

				ImGui::SameLine();

				static bool selectedCount = -1;
				if (*selected == o) selectedCount = count;
				if (ImGui::IsKeyReleased(ImGuiKey_F2))
					rename = selectedCount;
				else {
					ImGui::OpenPopupOnItemClick("ObjectContext", ImGuiPopupFlags_MouseButtonRight);
					if (ImGui::BeginPopup("ObjectContext"))
					{
						clicked = true;
						if (ImGui::MenuItem("Rename")) {
							rename = count;
						}
						InspectorCreateElementsList(&o->transform);
						ImGui::Separator();
						ImGui::EndPopup();
					}
				}

				ImGui::SetCursorPos(ImVec2(indent, ImGui::GetCursorPosY() + 2));
				ImGui::Text(o->m_name.c_str());
			}

			if (is_open) {
				if (o->transform.children.size() > 0)
				{
					int i = 0;
					for (auto& t : o->transform.children)
					{
						i++;
						InspectorElement(t->gameObject, selected, i, width, indent + 10.0f);
					}
				}

				ImGui::TreePop();
			}

			ImGui::EndGroup();

			ImGui::PopStyleColor(2);
			ImGui::PopID();
			ImGui::PopID();
			ImGui::PopID();
			return clicked;
		}

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
		
		void OpenSaveSceneDialog() {
			if (inPlayMode) return;

			if (SceneManager::currentScene) {
				wchar_t file[MAX_PATH];
				ZeroMemory(file, sizeof(MAX_PATH));

				OPENFILENAMEW props;
				ZeroMemory(&props, sizeof(props));
				props.lStructSize = sizeof(props);
				props.hwndOwner = getHwnd();
				props.lpstrFilter = L"Scene (*.scene)\0*.scene\0All Files (*.*)\0*.*\0";
				props.lpstrFile = file;
				props.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				props.lpstrDefExt = L"scene";
				props.nMaxFile = MAX_PATH;

				if (GetSaveFileName(&props)) {
					std::wstring wstr(file);
					std::string str(wstr.begin(), wstr.end());
					SceneManager::currentScene->Serialize(str);
				}
			}
		}

	public:

		static const wchar_t* WindowClassName() { return L"Helios Game Window"; };

		GameEngine()
		{
			/*wchar_t buff[MAX_PATH];
			ZeroMemory(buff, sizeof(buff));
			GetModuleFileName(NULL, buff, MAX_PATH);
			std::wstring::size_type pos = std::wstring(buff).find_last_of(L"\\/");
			projectPath = std::wstring(buff).substr(0, pos);*/

			projectPath = std::filesystem::current_path();

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

			Helios::SceneManager::AddScene(L"Test", [](Helios::Scene& scene) {
				Helios::GameObject::CreateMainCamera();
				auto& gm = Helios::GameObject::InstantiateObject("Test my man");
				gm.getTransform().setPosition(Vector2D(-5, -5));
				gm.AddComponent(new RectangleComponent(SolidBrush(Color(255, 255, 133))));
			});

			SceneManager::LoadScene(L"Test");
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
				OpenSaveSceneDialog();
			

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save", "Ctrl+S", false, !this->inPlayMode))
						OpenSaveSceneDialog();

					if (ImGui::MenuItem("Play", "", this->inPlayMode))
						this->inPlayMode != this->inPlayMode;

					if (ImGui::MenuItem("Exit")) {
						Application::Quit();
					}
					ImGui::EndMenu();
				}

				if (ImGui::Button("Play")) {
					
				}
				
				ImGui::EndMenuBar();
			}

			ImGui::ShowDemoWindow();

			static GameObject* selected = nullptr;

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
					auto currentViewportSize = graphics->m_renderTarget2D->GetSize();
					if (viewportSize.x != currentViewportSize.width || viewportSize.y != currentViewportSize.height)
					{
						SafeRelease(&graphics->m_renderTarget2D);
						SafeRelease(&graphics->m_renderTarget2Dtexture);
						SafeRelease(&graphics->pTextureView);
						graphics->CreateDXGIRenderTarget(viewportSize.x, viewportSize.y);
						WindowCordinates::SetSize(viewportSize.x, viewportSize.y);
					}
					ImGui::Image((void*)graphics->pTextureView, ImVec2(viewportSize));
				}
			}
			ImGui::End();
			ImGui::PopStyleVar();

			ProjectExplorerWindow(projectPath);

			if (ImGui::Begin("Hierarchy"))
			{
				const auto& width = ImGui::GetWindowWidth();
				bool clicked = false;

				for (int i = 0; i < GameObject::gameObjects.size(); i++)
				{
					clicked = InspectorElement(GameObject::gameObjects[i], &selected, i, width);
					//if (ImGui::Selectable(o->m_name.c_str(), o == selected, ImGuiSelectableFlags_AllowDoubleClick))
					//{
					//	if (ImGui::GetIO().MouseClicked[0]) {
					//		/*if (ImGui::BeginPopup("GameObject Editor")) {
					//			if (ImGui::MenuItem("Delete"))
					//			{
					//				GameObject::Destroy(*o);
					//			}
					//			ImGui::EndPopup();
					//		}*/

					//		ShowMessageBox(L"CLICKEDI TEM", std::wstring(o->m_name.begin(), o->m_name.end()).c_str());
					//	}
					//	selected = o;
					//}
				}

				if (!clicked && ImGui::BeginPopupContextWindow()) {
					InspectorCreateElementsList();
					ImGui::EndPopup();
				}

			}
			ImGui::End();

			if (ImGui::Begin("Inspector/Propeties"))
			{
				if (selected == nullptr)
					ImGui::Text("Nothing Selected!");
				else {

					/*static bool editing = false;
					if (editing)
					{
						static char buf[100];
						strcpy_s(buf, selected->m_name.c_str());
						if (ImGui::InputText("Name", buf, sizeof(buf), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							if (buf[0] != '\0') selected->m_name = buf;
							editing = false;
						}
					}
					else
					{
						ImGui::Text(selected->m_name.c_str());
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false))
							editing = true;
					}*/

					ImGui::Checkbox("##active", &selected->active);
					ImGui::SameLine();
					static char buf[100];
					strcpy_s(buf, selected->m_name.c_str());
					//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ImVec2(0, 0));
					ImGui::PushItemWidth(-1);
					if (ImGui::InputText("##label", buf, sizeof(buf), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (buf[0] != '\0') selected->m_name = buf;
					}
					ImGui::PopItemWidth();
					//ImGui::PopStyleVar();
					//ImGui::PopStyleColor(2);
					

					if (ImGui::CollapsingHeader("Transform"))
					{
						float* vec2Pos = selected->getTransform().getLocalPosition();
						ImGui::DragFloat2("Position", vec2Pos);
						selected->getTransform().setPosition(vec2Pos[0], vec2Pos[1]);

						float* vec2Size = selected->getTransform().getSize();
						ImGui::DragFloat2("Size", vec2Size);
						selected->getTransform().setSize(vec2Size[0], vec2Size[1]);
					}
					for (auto& o : selected->components)
					{

						if (typeid(RectangleComponent) == typeid(*o))
						{
						}
						else if (typeid(Camera) == typeid(*o)) {
							Camera* cam = (Camera*)o;
							float *c = cam->backgroundColor;
							if (ImGui::ColorEdit4("Clear Color", c)) {
								cam->SetBackgroundColor(c[0], c[1], c[2], c[3]);
							}
						}
					}

					ImVec2 lastSize = ImGui::GetWindowSize();
					ImVec2 lastPos = ImGui::GetWindowPos();

					if (ImGui::BeginPopupContextItem("Add Component"))
					{
						if (ImGui::Button("Rectangle Renderer")) {
							selected->AddComponent(new RectangleComponent(SolidBrush(Color::White)));
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

					ImGui::Dummy(ImVec2(0.0f, 50.0f));
					ImGui::Spacing();
					ImGui::SameLine((lastSize.x / 2) - 75.0f);
					if (ImGui::Button("Add Component", ImVec2(150.0f, 0.0f)))
					{
						ImGui::OpenPopup("Add Component");
					}
				}
			}

			ImGui::End();

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

			OnGUI();		

			ImGui::EndFrame();
			ImGui::Render();
			
			graphics->m_deviceContext->OMSetRenderTargets(1, &graphics->m_mainRenderTarget, NULL);
			graphics->ClearRenderTarget(0.5f, 0.0f, 1.0f);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

#pragma region Rendering
			if (SceneManager::currentScene) {
				graphics->m_renderTarget2D->BeginDraw();

				SceneManager::Render(*graphics);

				Render(*graphics);

				graphics->m_renderTarget2D->EndDraw();
			}
#pragma endregion

			graphics->EndFrame();

		}
		
		~GameEngine() {
			ImGui::DestroyContext();
		}

	};
}

Helios::Application* Helios::CreateApplication() { return new Helios::GameEngine(); }