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
#include "Wallnut/Utils/ErrorHandling.h"

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
			
			/*static ImGuiIO& io = ImGui::GetIO();
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
			}*/

			static GameObject* selected = nullptr;

			if (ImGui::Begin("Game"))
			{
				if(graphics->pTextureView)
					ImGui::Image((void*)graphics->pTextureView, ImVec2(500, 500));
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

			//ImGui::End();

			ImGui::End();
		}

		void RenderLoop() override {
			static auto& io = ImGui::GetIO();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			OnGUI();

#pragma region Rendering
			if (SceneManager::currentScene) {
				graphics->m_d2renderTarget->BeginDraw();

				SceneManager::Render(*graphics);

				Render(*graphics);

				graphics->m_d2renderTarget->EndDraw();
			}
#pragma endregion


			

			ImGui::EndFrame();
			ImGui::Render();
			
			//graphics->m_d3d10Device1->OMSetRenderTargets(1, &graphics->m_d3renderTarget_10, NULL);
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
				graphics->m_d2renderTarget->BeginDraw();

				SceneManager::Render(*graphics);

				Render(*graphics);

				graphics->m_d2renderTarget->EndDraw();
			}
#pragma endregion

			if (InputManager::IsKeyPressedDown(WN_KEY_S) && InputManager::IsKeyPressedDown(WN_KEY_SHIFT_LEFT)) {
				ID3D11Texture2D* frameBuffer;
				if (SUCCEEDED(graphics->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&frameBuffer))))
				{
					ID3D11Texture2D* texture;
					D3D11_TEXTURE2D_DESC textureDesc;
					ZeroMemory(&textureDesc, sizeof(textureDesc));

					textureDesc.Width = 1000;
					textureDesc.Height = 1000;
					textureDesc.MipLevels = 1;
					textureDesc.ArraySize = 1;
					textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					textureDesc.SampleDesc.Count = 1;
					textureDesc.Usage = D3D11_USAGE_DEFAULT;
					textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = 0;
					textureDesc.MiscFlags = 0;

					HRESULT hr = graphics->m_device->CreateTexture2D(&textureDesc, NULL, &texture);


					if (texture) {

						D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
						renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
						renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
						renderTargetViewDesc.Texture2D.MipSlice = 0;

						static ID3D11RenderTargetView* refRen;
						graphics->m_device->CreateRenderTargetView(texture, &renderTargetViewDesc, &refRen);

						D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
						shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
						shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
						shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
						shaderResourceViewDesc.Texture2D.MipLevels = 1;

						graphics->m_deviceContext->CopyResource(texture, frameBuffer);

						HRESULT hr;
						if (SUCCEEDED(hr = graphics->m_device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &graphics->pTextureView))) {
							//MessageBox(getHwnd(), L"SHADER RESOURCE VIEW", L"SUCCESS", MB_ICONERROR);

							//graphics->m_deviceContext->CopyResource(texture, graphics->d2RenderTargetTexture);

							//graphics->m_deviceContext->OMSetRenderTargets(1, &refRen, NULL);
							/*float c[4] = { 0.5f, 1.0f, 1.0f, 0.0f };
							graphics->m_deviceContext->ClearRenderTargetView(refRen, c);*/

							//graphics->m_deviceContext->CopyResource(texture, frameBuffer);

							//graphics->m_deviceContext->Map()

							/*if (SUCCEEDED(graphics->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&frameBuffer))))
							{
								graphics->m_deviceContext->CopyResource(texture, graphics->d2RenderTargetTexture);
							}*/

						}
						else {
							//MessageBox(getHwnd(), GetErrorAsReadable(hr).c_str(), L"SHADER RESOURCE VIEW FAIL", MB_ICONERROR);
						}
						//pTexture->Release();

					}
				}
			}

			graphics->EndFrame();

		}
		
		~GameEngine() {
			ImGui::DestroyContext();
		}

	};
}

Wallnut::Application* Wallnut::CreateApplication() { return new Wallnut::GameEngine(); }