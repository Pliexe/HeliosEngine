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

#include <d2d1_3.h>

//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx10.h>

#include "Components/CameraInspector.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Wallnut;

namespace Wallnut {
	class GameEngine : public Wallnut::Application {
	private:

		LRESULT CALLBACK WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override
		{
			if (ImGui_ImplWin32_WndProcHandler(m_hWnd, uMsg, wParam, lParam))
				return true;
			else Application::WindowProc(uMsg, wParam, lParam);
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
			ImGui_ImplDX10_Init(graphics->m_d3Device);

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
			ImGui::PopStyleVar();
			ImGui::PopStyleVar(2);

			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit")) {
						Application::Quit();
					}
					ImGui::EndMenu();
				}
				
				ImGui::EndMenuBar();
			}

			static GameObject* selected = nullptr;

			if (ImGui::Begin("Game"))
			{
				if(graphics->pTextureView)
					ImGui::Image((void*)graphics->pTextureView, ImVec2(100, 100));
			}
			ImGui::End();

			if (ImGui::Begin("Hierarchy"))
			{
				for (auto& o : GameObject::gameObjects)
				{
					if (ImGui::Selectable(o->m_name, o == selected))
					{
						selected = o;
					}
				}

				if (ImGui::BeginPopupContextWindow()) {
					if (ImGui::MenuItem("Create Empty"))
					{
						GameObject::InstantiateObject("Empty");
					}
					ImGui::EndPopup();
				}

			}
			ImGui::End();

			if (ImGui::Begin("Inspector/Propeties"))
			{
				if (selected == nullptr)
					ImGui::Text("Nothing Selected!");
				else {
					ImGui::Text(selected->m_name);
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

						/*if (typeid(RectangleComponent) == typeid(*o))
						{
						}
						else if (typeid(Camera) == typeid(*o)) {
							CameraInspectorGUI();
						}*/
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

		void RenderLoop() override {
			static auto& io = ImGui::GetIO();

			ImGui_ImplDX10_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			OnGUI();

#pragma region Rendering
			if (SceneManager::currentScene) {
				graphics->renderTarget->BeginDraw();

				SceneManager::Render(*graphics);

				Render(*graphics);

				graphics->renderTarget->EndDraw();
			}
#pragma endregion

			ImGui::EndFrame();
			ImGui::Render();
			
			graphics->m_d3Device->OMSetRenderTargets(1, &graphics->m_d3RenderTarget, NULL);
			//graphics->ClearRenderTarget(0.5f, 0.0f, 1.0f);
			ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			graphics->EndFrame();



			ID3D10Texture2D* frameBuffer;
			if (SUCCEEDED(graphics->m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<LPVOID*>(&frameBuffer))))
			{
				if (InputManager::IsKeyPressedDown(WN_KEY_S) && InputManager::IsKeyPressedDown(WN_KEY_SHIFT_LEFT)) {
					ID3D10Texture2D* pNewTexture = NULL;

					D3D10_TEXTURE2D_DESC desc;
					frameBuffer->GetDesc(&desc);
					desc.BindFlags = 0;
					desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
					desc.Usage = D3D10_USAGE_STAGING;

					HRESULT hr = graphics->m_d3Device->CreateTexture2D(&desc, NULL, &pNewTexture);

					if (pNewTexture) {
						graphics->m_d3Device->CopyResource(pNewTexture, frameBuffer);
						
						// Load from disk into a raw RGBA buffer
						int image_width = 0;
						int image_height = 0;
						unsigned char* image_data = stbi_load("resources/unknown.png", &image_width, &image_height, NULL, 4);
						if (image_data != NULL)
						{
							// Create texture
							D3D10_TEXTURE2D_DESC desc;
							ZeroMemory(&desc, sizeof(desc));
							desc.Width = image_width;
							desc.Height = image_height;
							desc.MipLevels = 1;
							desc.ArraySize = 1;
							desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
							desc.SampleDesc.Count = 1;
							desc.Usage = D3D10_USAGE_DEFAULT;
							desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
							desc.CPUAccessFlags = 0;

							ID3D10Texture2D* pTexture = NULL;
							D3D10_SUBRESOURCE_DATA subResource;
							subResource.pSysMem = image_data;
							subResource.SysMemPitch = desc.Width * 4;
							subResource.SysMemSlicePitch = 0;
							graphics->m_d3Device->CreateTexture2D(&desc, &subResource, &pTexture);


							// Create texture view
							D3D10_SHADER_RESOURCE_VIEW_DESC1 srvDesc;
							ZeroMemory(&srvDesc, sizeof(srvDesc));
							srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
							srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2D;
							srvDesc.Texture2D.MipLevels = desc.MipLevels;
							srvDesc.Texture2D.MostDetailedMip = 0;

							HRESULT hr = graphics->m_d3Device->CreateShaderResourceView1(pNewTexture, &srvDesc, &graphics->pTextureView);
							if (graphics->pTextureView) {
								std::cout << "Let's go" << std::endl;
							}
							else {
								if (hr == E_INVALIDARG)
									std::cout << "It's null: " << "Invalid Params" << std::endl;
								else
									std::cout << "It's null: " << hr << std::endl;
							}
							pTexture->Release();
						}

						
						pNewTexture->Release();
					}
				}
				

				//D3D10_TEXTURE2D_DESC bbDesc;
				//frameBuffer->GetDesc(&bbDesc);

				//ID3D10Texture2D* pTexture = NULL;
				//graphics->m_d3Device->CopyResource(pTexture, frameBuffer);
				//D3D10_TEXTURE2D_DESC bbDesc2;
				//pTexture->GetDesc(&bbDesc2);

				//std::cout << "AAA: " << bbDesc.Width << std::endl;

				//// Create texture view
				//D3D10_SHADER_RESOURCE_VIEW_DESC srv_desc;
				//ZeroMemory(&srv_desc, sizeof(srv_desc));
				//srv_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
				//srv_desc.Format = bbDesc.Format;

				///*srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				//srv_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;*/

				//HRESULT hr = graphics->m_d3Device->CreateShaderResourceView(frameBuffer, &srv_desc, &graphics->pTextureView);
				//if (SUCCEEDED(hr)) {
				//	pTexture->Release();
				//}


				//frameBuffer->Release();
			}

		}
		
		~GameEngine() {
			ImGui::DestroyContext();
		}

	};
}

Wallnut::Application* Wallnut::CreateApplication() { return new Wallnut::GameEngine(); }