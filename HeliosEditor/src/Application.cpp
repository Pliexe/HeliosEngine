#define HELIOS_EDITOR

#include <iostream>
#include <windows.h>

#include <Helios.h>
#include <Helios/Resources/EngineTexture.h>
#include <Helios/Resources/Color.h>
#include <Helios/Graphics/Renderer2D.h>

#include <Helios/Utils/ErrorHandling.h>

#include <d2d1_3.h>

#include "Helios/Utils/ErrorHandling.h"
#include <filesystem>

#include <Panels/ProjectExplorer.h>
#include "ProjectManager.h"

#include "Icons.h"
#include "Panels/BasePanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"

#include "HeliosEditor_Macros.h"
#include <imgui_impl_win32.cpp>
#include <imgui_impl_dx11.cpp>
#include "Helios/Translation/Matrix.h"
#include <Helios/Translation/Quanterion.h>
#include <sstream>
#include <Helios/Graphics/Framebuffer.h>

#include "AssetRegistry.h"
#include <Helios/Resources/Shader.h>

static std::filesystem::path currentScene;
StartupConfig startupConfig;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Helios;

static SceneCamera editorCamera({ { 0.0f, 0.0f, -5.0f } });

static Ref<Framebuffer> editorFrame;
static Ref<Framebuffer> gameFrame;

static Vector2 editorCameraRotation = { 0.0f, 0.0f };

enum class EditorMode
{
	Editor,
	Paused,
	Playing
};

static EditorMode currentMode = EditorMode::Editor;

bool isGameSceneActive = false;
bool isEditorSceneActive = false;

struct TransformVertex
{
	Vector3 position;
	Color color;
};

static const TransformVertex transformMoveVertices[] = {
	{ {  0.0f, 1.0f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ {  0.2f, 0.8f,  0.2f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { -0.2f, 0.8f,  0.2f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ {  0.2f, 0.8f, -0.2f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { -0.2f, 0.8f, -0.2f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
};

static const unsigned short transformMoveIndecies[] = {
	0, 1, 2
};

static Ref<VertexBuffer> transformMoveVertexBuffer;
static Ref<IndexBuffer> transformMoveIndexBuffer;
static Ref<Shader> transformShader;

void InitTransformBuffers()
{
	transformMoveVertexBuffer = VertexBuffer::Create(&transformMoveVertices, std::size(transformMoveVertices) * sizeof(TransformVertex), Helios::BufferUsage::Dynamic);
	transformMoveIndexBuffer = IndexBuffer::Create((const unsigned short*)&transformMoveIndecies, std::size(transformMoveIndecies));
	transformShader = CreateRef<Shader>(Shader("Transform", {
		{ "Position", Shader::DataType::Float3 },
		{ "Color", Shader::DataType::Float4 },
	}));
}

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

				ICON_PLAY_WHITE = std::make_shared<EngineTexture>(EngineTexture("resources/play_icon_white.png"));
				ICON_PAUSE_WHITE = std::make_shared<EngineTexture>(EngineTexture("resources/pause_icon_white.png"));
				ICON_STOP_WHITE = std::make_shared<EngineTexture>(EngineTexture("resources/stop_icon_white.png"));
			}
			catch (std::runtime_error e) {
				ShowMessage("Error loading icons!", e.what(), MB_ICONERROR, true);
				exit(-100);
			}

			editorFrame = Framebuffer::Create(300, 300);
			editorFrame->AddDepthStencil();
			gameFrame = Framebuffer::Create(300, 300);
			gameFrame->AddDepthStencil();

			InitTransformBuffers();

			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(m_hWnd);
			ImGui_ImplDX11_Init(graphics->m_device, graphics->m_deviceContext);

			

			AssetRegistry::Init();

			using namespace Helios;

			//Project::TryLoad();

			Helios::SceneManager::AddScene("Test", [](Helios::Scene& scene) {
				Helios::GameObject::CreateMainCamera();
			});

			SceneManager::LoadScene("Test");
		}

		void OnGUI() {			
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

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New Scene", "", false, currentMode == EditorMode::Editor))
					{
						InspectorPanel::Reset();
						Project::LoadSampleScene();
					}
					if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, currentMode == EditorMode::Editor))
					{
						InspectorPanel::Reset();
						Project::OpenSceneDialog();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Save", "Ctrl+S", false, currentMode == EditorMode::Editor))
						Project::SaveScene();
					
					if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, currentMode == EditorMode::Editor))
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

				ImVec2 pos = ImGui::GetWindowPos();
				ImVec2 size = ImGui::GetWindowSize();

				ImGui::SetNextWindowPos(ImVec2(pos.x + size.x / 2.0f - 50.0f, pos.y));
				//ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 255));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
				ImGui::BeginChild("statePanel", ImVec2(58, 25));

				ImVec4 col = (currentMode == EditorMode::Playing ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
				if (ImGui::ImageButton("playBtn", currentMode == EditorMode::Editor ? *ICON_PLAY_WHITE : *ICON_STOP_WHITE, ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 1), col))
				{
					switch (currentMode)
					{
					case EditorMode::Editor:
						currentMode = EditorMode::Playing;
						break;
					case EditorMode::Paused:
						currentMode = EditorMode::Editor;
						break;
					case EditorMode::Playing:
						currentMode = EditorMode::Editor;
						break;
					}
				}
				ImGui::SameLine();
				col = (currentMode == EditorMode::Paused ? ImVec4(0.5f, 0.5f, 0.5f, 1.0f) : ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
				if (ImGui::ImageButton("pauseBtn", currentMode == EditorMode::Paused ? *ICON_PLAY_WHITE : *ICON_PAUSE_WHITE, ImVec2(25, 25), ImVec2(0, 0), ImVec2(1, 1), col))
				{
					switch (currentMode)
					{
					case EditorMode::Paused:
						currentMode = EditorMode::Playing;
						break;
					case EditorMode::Playing:
						currentMode = EditorMode::Paused;
						break;
					}
				}

				ImGui::EndChild();
				ImGui::PopStyleVar();
				//ImGui::PopStyleColor();

				//ImGui::ImageButton((ImTextureID)*ICON_PLAY_WHITE, ImVec2(20, 20));
				/*if ()
					currentMode = EditorMode::Playing;
				}*/
				
				ImGui::EndMenuBar();
			}

			for (auto& panel : panels)
			{
				bool is_open = true;
				std::string tmp = panel->GetName();
				if (ImGui::Begin(panel->GetName().c_str(), &is_open)) {
					panel->OnUpdate();
				}
				ImGui::End();
			}
			
			AssetRegistry::ShowRegistryWindow();
			AssetRegistry::ShowTextureSelect();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
			if (ImGui::Begin("Scene"))
			{
				if (editorFrame->IsSet())
				{
					ImVec2 viewportSize = ImGui::GetContentRegionAvail();
					auto currentViewportSize = editorFrame->GetSize();
					if (viewportSize.x != currentViewportSize.x || viewportSize.y != currentViewportSize.y)
					{
						 editorFrame->Resize(viewportSize.x, viewportSize.y);
					}
					ImGui::Image(editorFrame->GetTextureID(), ImVec2(viewportSize));

					//ImVec2(viewportSize)

					//ImGui::Image((void*)editorFrame->GetTextureID(), );
					// ImGui::Image((void*)editorFrame->GetTextureID(), ImVec2(300, 300));
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
					static ImVec2 origin;
					static bool mActive = false;

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
						origin = ImGui::GetMousePos();
						mActive = true;
					}

					if (mActive)
					{
						std::cout << "Mouse Is Held" << std::endl;
						ImVec2 current = ImGui::GetMousePos();

						auto difference = Vector2(origin.x, origin.y) - Vector2(current.x, current.y);
						auto normalizedCordinates = -(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize();

						editorCamera.HandleControls(
							-(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize()
						);

						SetCursorPos(origin.x, origin.y);
					}

					if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
						mActive = false;
						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					}
				}

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R)) {
					editorCamera.Reset();
				}
				isEditorSceneActive = true;
			} else isEditorSceneActive = false;
			ImGui::End();
			ImGui::PopStyleVar();


			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
			if (ImGui::Begin("Game", 0, ImGuiWindowFlags_MenuBar))
			{
				if (ImGui::BeginMenuBar())
				{
					static bool depthG = true;
					if (ImGui::Checkbox("Depth", &depthG))
					{
						if (depthG)
							gameFrame->AddDepthStencil();
						else gameFrame->RemoveDepthStencil();
					}

					ImGui::EndMenuBar();
				}
				if (gameFrame->IsSet())
				{
					ImVec2 viewportSize = ImGui::GetContentRegionAvail();
					auto currentViewportSize = gameFrame->GetSize();
					if (viewportSize.x != currentViewportSize.x || viewportSize.y != currentViewportSize.y)
					{
						 gameFrame->Resize(viewportSize.x, viewportSize.y);
					}
					ImGui::Image(gameFrame->GetTextureID(), ImVec2(viewportSize));
				}

				isGameSceneActive = true;
			} else isGameSceneActive = false;
			ImGui::End();
			ImGui::PopStyleVar();
			

			ProjectExplorerWindow(Project::GetAssetsPath());

			ImGui::End();
		}

		void OnUpdate() override {
			if (currentMode == EditorMode::Playing)
			{
				// Physics, Scripts etc
			}
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


			if(isGameSceneActive) {
				gameFrame->Bind();
				gameFrame->ClearColor({ 0.0f, 0.0f, 0.0f });

				if (SceneManager::currentScene)
					SceneManager::currentScene->OnUpdateRuntime();

				gameFrame->Unbind(); 
			}

			if(isEditorSceneActive) {
				editorFrame->Bind();
				editorFrame->ClearColor({ 0.0f, 0.0f, 0.0f });

	#pragma region Rendering
				if (SceneManager::currentScene) {
					SceneManager::currentScene->OnUpdateEditor(editorCamera);
				}
	#pragma endregion

				transformMoveVertexBuffer->Bind();
				transformMoveIndexBuffer->Bind();
				transformShader->Bind();

				Graphics::instance->m_deviceContext->DrawIndexed(std::size(transformMoveIndecies), 0u, 0u);

				editorFrame->Unbind();
			}


			graphics->m_deviceContext->OMSetRenderTargets(1, &graphics->m_mainRenderTarget, NULL);

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