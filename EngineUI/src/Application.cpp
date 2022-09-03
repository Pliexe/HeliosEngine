#include <iostream>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <Wallnut.h>
#include <Wallnut/Application.h>
#include <Wallnut/Time.h>
#include <Wallnut/GameObjects/GameObject.h>
#include <Wallnut/GameObjects/Components/RectangleComponent.h>
#include <Wallnut/GameObjects/Components/EllipseComponent.h>
#include <Wallnut/Resources/Brushes/SolidBrush.h>
#include <Wallnut/Resources/Color.h>
#include <Wallnut/GameObjects/Components/Camera.h>

#include <Wallnut/Utils/ErrorHandling.h>

#include <d2d1_3.h>

//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx10.h>

#include "Components/CameraInspector.h"
#include "Wallnut/Utils/ErrorHandling.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Wallnut;

namespace Wallnut {
	class GameEngine : public Wallnut::Application {
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

		GameEngine()
		{
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
		}

		// Inherited via Application
		virtual void Init() override
		{
			
			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(m_hWnd);
			ImGui_ImplDX11_Init(graphics->m_device, graphics->m_deviceContext);

			using namespace Wallnut;

			Wallnut::SceneManager::AddScene(L"Test", [](Wallnut::Scene& scene) {
				Wallnut::GameObject::CreateMainCamera();
				auto& gm = Wallnut::GameObject::InstantiateObject("Test my man");
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
				
				ImGui::EndMenuBar();
			}

			ImGui::ShowDemoWindow();

			static GameObject* selected = nullptr;

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

			if (ImGui::Begin("Game View 2"))
			{
				if (graphics->pTextureView)
					ImGui::Image((void*)graphics->pTextureViewtest, ImVec2(500, 500));
			}
			ImGui::End();

			if (ImGui::Begin("Explorer")) {
				static bool rootDir = ".";
				static bool currentDir = ".";
			}
			ImGui::End();

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
						float* vec2Pos = selected->getTransform().getPosition();
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

Wallnut::Application* Wallnut::CreateApplication() { return new Wallnut::GameEngine(); }