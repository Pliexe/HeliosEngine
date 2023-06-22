#include "ScenePanel.h"

#include "imgui.h"
#include "InspectorPanel.h"
#include "Helios/Input/InputManager.h"
#include "Helios/Input/KeyCodes.h"
#include "Application.h"

namespace Helios
{
	
	void ScenePanel::RenderFramebuffer()
	{
		m_Framebuffer->Bind();

		m_Framebuffer->ClearBuffer( 0u, { 0.0f, 0.0f, 0.0f } );
		m_Framebuffer->ClearBuffer( 1u, { 0.f, 0.f, -1.0f } );
		m_Framebuffer->ClearDepthStencil();

		SceneRegistry::OnEditorRender(m_EditorCamera);

		m_Framebuffer->Unbind();
	}

	void ScenePanel::ResetControls()
	{
		m_EditorCamera.Reset();
	}

	void ScenePanel::OnUpdate()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if(ImGui::Begin(GetName().c_str(), &m_window_open, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::Checkbox("Show ID Frame", &showIdFrame);
				//static bool wireframe = false;
				//ImGui::Checkbox("Wireframe", &wireframe);
				/*if (wireframe)
						this->currentRSState = RSState::Wireframe;
					else
						this->currentRSState = RSState::Normal;*/

				ImGui::Checkbox("Show Gizmos", &show_gizmos);

				ImGui::EndMenuBar();
			}

			auto imgPos = ImGui::GetCursorPos();
			auto mousePos = ImGui::GetMousePos();
			auto windowPos = ImGui::GetWindowPos();
			auto x = mousePos.x - windowPos.x - imgPos.x;
			auto y = mousePos.y - windowPos.y - imgPos.y;

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			auto currentvpsize = m_Framebuffer->GetSize();
			if (viewportSize.x != currentvpsize.width || viewportSize.y != currentvpsize.height)
			{
				m_Framebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
				m_EditorCamera.SetViewportSize(viewportSize.x, viewportSize.y);
			}
			ImGui::Image(m_Framebuffer->GetTextureID((uint32_t)showIdFrame), ImVec2(viewportSize));

			ImGui::Begin("Mouse Location Stats");

			ImGui::Text("Mouse X: %f, Y: %f", x, y);

			if (x >= 0 && y >= 0 && x < m_Framebuffer->GetWidth() && y < m_Framebuffer->GetHeight()) {
				ImGui::Text("ID: %f", std::round(m_Framebuffer->GetPixelColor(1u, x, y).r));
				Color color = m_Framebuffer->GetPixelColor(0u, x, y);
				ImGui::Text("R: %f, G: %f, B: %f, A: %f", color.r, color.g, color.b, color.a);
			}
			else {
				ImGui::Text("ID: -");
				ImGui::Text("R: -, G: -, B: -, A: -");
			}

			ImGui::Text("Mouse to World: %s", m_EditorCamera.ScreenToWorldPoint(x, y, 5.0f).to_string().c_str());


			ImGui::End();



			if (InspectorPanel::GetInstance().GetType() == InspectorPanel::SelectedType::Entity)
					{

						Entity entity = std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle());
						if (entity.IsNotNull())
						{
							if (entity.HasComponent<TransformComponent>())
							{
								GizmosRenderer::DrawTool(Transform(entity).GetWorldTransformCache().GetModelMatrix(), GizmosRenderer::ToolType::Move, current_tool);
							}
							if (entity.HasComponent<MeshRendererComponent>())
							{
								Ref<Mesh> mesh = entity.GetComponent<MeshRendererComponent>().mesh;
								ImGui::Begin("Mesh Stats");
								auto transform = Transform(entity);

								auto dir = (transform.GetWorldPosition() - m_EditorCamera.GetPosition()).normalize();
								auto cameraForward = m_EditorCamera.GetForward();

								auto trans = entity.GetComponent<TransformComponent>();

								auto proj = m_EditorCamera.GetViewProjection();

								float alpha = std::acos(Vector3::Dot(cameraForward, dir)) / (cameraForward.magnitude() * dir.magnitude());
								float C = Vector3::Distance(transform.GetWorldPosition(), m_EditorCamera.GetPosition());

								float A = C * std::sin(alpha);
								float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

								//float angle = std::acos(Vector3::Dot(camTrans.Position, trans.Position)) / (camTrans.Position.magnitude() * trans.Position.magnitude());

								ImGui::Text("Angle: %f", alpha);
								ImGui::Text("C: %f, B: %f, A: %f", C, B, A);
								ImGui::Text("Direction: %f %f %f", dir.x, dir.y, dir.z);
								ImGui::Text("Forward: %f %f %f", cameraForward.x, cameraForward.y, cameraForward.z);

								uint32_t indeciesCount = mesh->getIndexCount();

								ImGui::Text("Vertex Count: %d", mesh->getVertexCount());
								ImGui::Text("Index Count: %d", indeciesCount);

								ImGui::Text(mesh->getIndexBuffer()->m_DataStr.c_str());

								ImGui::End();
							}
						}
					}


				static bool pressed = false;
				static ImVec2 origin;
				static TransformComponent originTransform;
				static TransformComponent originWorldTransform;
				static Entity originEntity;



				if (Helios::InputManager::IsKeyPressed(HL_KEY_MOUSE_LEFT) && ImGui::IsWindowHovered())
				//if (Helios::InputManager::IsKeyPressed(HL_KEY_MOUSE_LEFT) && ImGui::IsWindowFocused())
				{
					// Check if in bounds
					if (current_tool == GizmosRenderer::Tool::None && x >= 0 && y >= 0 && x < m_Framebuffer->GetWidth() && y < m_Framebuffer->GetHeight())
					{

						Color color = m_Framebuffer->GetPixelColor(0u, x, y);
						Color entId = m_Framebuffer->GetPixelColor(1u, x, y);

						Int4 clicked_pixel = m_Framebuffer->GetPixelInt4(1u, x, y);
						int32_t mode_or_index = clicked_pixel.z;

						//MessageBoxA( NULL, std::to_string(mode_or_index).c_str(), "Info", MB_OK );

						if (mode_or_index >= 0)
						{
							uint64_t entityId = ((*(uint64_t*)&clicked_pixel.x) & 0xffffffff) | ((*(uint64_t*)&clicked_pixel.y) << 32);

							WeakRef<Scene> scene = SceneRegistry::GetSceneByIndex(mode_or_index);

							Entity entity = { (entt::entity)entityId, scene.lock().get() };

							InspectorPanel::GetInstance() << entity;
							InspectorPanel::FocusMain();
						} else if (mode_or_index == -1)
						{
							InspectorPanel::GetInstance().Reset();
						} else if(mode_or_index >= -13 || current_tool != GizmosRenderer::Tool::None)
						{
							if (static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).inspector.GetType() == InspectorPanel::SelectedType::Entity)
							{
								Entity obj = std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle());
								//TransformComponent& transform = obj.GetComponent<TransformComponent>();
								Transform transform = Transform(obj);

								if (!pressed)
								{
									origin = ImGui::GetMousePos();
									originTransform = transform.GetTransformComponent();
									originWorldTransform = transform.GetWorldTransformCache();
									pressed = true;
									current_tool = (GizmosRenderer::Tool)mode_or_index;
									originEntity = obj;
								}
							}
						}
					}
				}
				else { pressed = false; current_tool = GizmosRenderer::Tool::None; }

				if (current_tool != GizmosRenderer::Tool::None && originEntity.IsValid())
				{
					Transform transform(originEntity);
					//TransformComponent& transform = originEntity.GetComponent<TransformComponent>();

					switch (current_tool)
					{
					case GizmosRenderer::Tool::MoveX:
						{
							auto cameraForward = m_EditorCamera.GetForward();
							auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

							float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());

							float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

							float pA = pC * std::sin(palpha);
							float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

							Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
							auto forw = originWorldTransform.Forward();
							auto dir = (originWorldTransform.Position - mvPoint).normalize();
							//float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

							float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());

							float C = Vector3::Distance(originWorldTransform.Position, mvPoint);

							float a1 = C * std::sin(alpha);
							//float length = Vector3::Distance(originWorldTransform.Position, mvPoint * std::cos(alpha));
							//float length = std::cos(alpha) * Vector3::Distance(originWorldTransform.Position, mvPoint);

							std::cout << "Alpha: " << alpha << ", Cos: " << std::cos(alpha) << ", Length: " << a1 << std::endl;
							
							GizmosRenderer::DrawLine(originTransform.Right() * 1000.0f + originTransform.Position, originTransform.Right() * -1000.0f + originTransform.Position, 0.5f, Color::Red, -1, GizmosRenderer::LineMode::Solid);


							transform.SetLocalPosition(originTransform.Position + originTransform.Right() * a1 * (dir.x > 0.0f ? -1.0f : 1.0f));

							// DrawAngle mvPoint and originWorldTransform.Position
							// GizmosRenderer::DrawAngle(originWorldTransform.Position, mvPoint, dir, 5.0f, Color::Red, Matrix4x4::Identity());

							GizmosRenderer::DrawLine(originWorldTransform.Position, originWorldTransform.Position + forw * a1, 0.5f, Color::Green, -1, GizmosRenderer::LineMode::Solid);
							GizmosRenderer::DrawLine(originWorldTransform.Position, originWorldTransform.Position + originWorldTransform.Right() * a1, 0.5f, Color::Green, -1, GizmosRenderer::LineMode::Solid);
							GizmosRenderer::DrawLine(originWorldTransform.Position, originWorldTransform.Position + dir * C, 0.5f, Color::Red, -1, GizmosRenderer::LineMode::Solid);
							GizmosRenderer::DrawLine(originWorldTransform.Position, mvPoint, 0.5f, Color::Yellow, -1, GizmosRenderer::LineMode::Rounded_Dash_Dot_Dot);
							GizmosRenderer::DrawLine(originWorldTransform.Position + originWorldTransform.Right() * a1 * (alpha > 0.0f ? -1.0f : 1.0f), mvPoint, 0.5f, Color::Green, -1, GizmosRenderer::LineMode::Dash_Dot);

							

							/*auto cameraForward = m_EditorCamera.GetForward();
							auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

							float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
							float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

							float pA = pC * std::sin(palpha);
							float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

							Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
							auto forw = originWorldTransform.Forward();
							auto dir = (originWorldTransform.Position - mvPoint).normalize();
							float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

							float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


							float A = C * std::sin(alpha);
							float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

							std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

							tool_lines.emplace(Line{ originTransform.Right() * 1000.0f + originTransform.Position, originTransform.Right() * -1000.0f + originTransform.Position, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });

							transform.SetLocalPosition(originTransform.Position + originTransform.Right() * A * (dir.x > 0.0f ? -1.0f : 1.0f));*/
						}
						break;
					case GizmosRenderer::Tool::MoveZ:
						{
							auto cameraForward = m_EditorCamera.GetForward();
							auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

							float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
							float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

							float pA = pC * std::sin(palpha);
							float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

							Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
							auto forw = originWorldTransform.Forward();
							auto dir = (originWorldTransform.Position - mvPoint).normalize();
							float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

							float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


							float A = C * std::sin(alpha);
							float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

							std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

							GizmosRenderer::DrawLine (originTransform.Forward() * 1000.0f + originTransform.Position, originTransform.Forward() * -1000.0f + originTransform.Position, 0.5f, Color::Green, -1, GizmosRenderer::LineMode::Solid);

							transform.SetLocalPosition(originTransform.Position - originTransform.Forward() * A * (dir.x > 0.0f ? -1.0f : 1.0f));
						}
						break;
					case GizmosRenderer::Tool::MoveY:
						{
							auto cameraForward = m_EditorCamera.GetForward();
							auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

							float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
							float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

							float pA = pC * std::sin(palpha);
							float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

							Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
							auto forw = originWorldTransform.Forward();
							auto dir = (originWorldTransform.Position - mvPoint).normalize();
							float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

							float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


							float A = C * std::sin(alpha);
							float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

							std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;
							
							GizmosRenderer::DrawLine(originTransform.Up() * 1000.0f + originTransform.Position, originTransform.Up() * -1000.0f + originTransform.Position, 0.5f, Color::Blue, -1, GizmosRenderer::LineMode::Solid);

							transform.SetLocalPosition(originTransform.Position + originTransform.Up() * A * (dir.x > 0.0f ? -1.0f : 1.0f));
						}
						break;
					case GizmosRenderer::Tool::MoveXYZ:
						{
							auto cameraForward = m_EditorCamera.GetForward();
							auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

							float alpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
							float C = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

							float A = C * std::sin(alpha);
							float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

							auto newLocation = m_EditorCamera.ScreenToWorldPoint(x, y, B);
							transform.SetLocalPosition(newLocation);

							GizmosRenderer::DrawLine( originTransform.Position, newLocation, 0.8f, Color::BlanchedAlmond, -1, GizmosRenderer::LineMode::Rounded_Dashed);

							break;
						}
					default: break;
					}

					if (InputManager::IsKeyPressed(HL_KEY_ESCAPE)) { current_tool = GizmosRenderer::Tool::None; transform.GetTransformComponent() = originTransform; }

				}

				//static bool notf = true;

				//if (!SceneRegistry::GetCurrentScene().lock()->IsPrimaryCameraSet()) {
				//	if (notf) {
				//		auto pos = ImGui::GetWindowPos();
				//		auto Size = ImGui::GetWindowSize();
				//		ImGui::SetNextWindowFocus();
				//		ImGui::SetNextWindowBgAlpha(1.0f);
				//		ImGui::SetWindowFontScale(2);
				//		ImGui::SetNextWindowPos(ImVec2(pos.x + (Size.x / 2.0f) - 200.0f, pos.y + (Size.y / 2.0f) - 100.0f));
				//		ImGui::BeginChild("MissingCameraText", ImVec2(400, 200), true);
				//		//ImGui::Button("Test");
				//		//ImGui::SetCursorPos(ImVec2(0, 0));
				//		ImGui::Text("No CameraComponent Present for rendering!");
				//		if (ImGui::Button("Close")) {
				//			notf = false;
				//		}
				//		ImGui::EndChild();
				//	}
				//}
				//else notf = true;

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
						//std::cout << "Mouse Is Held" << std::endl;
						ImVec2 current = ImGui::GetMousePos();

						auto difference = Vector2(origin.x, origin.y) - Vector2(current.x, current.y);
						auto normalizedCordinates = -(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize();
						
						m_EditorCamera.HandleMovement(-(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize());

						SetCursorPos(origin.x, origin.y);
					}

					if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
						mActive = false;
						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					}
				}

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R)) {
					m_EditorCamera.Reset();
				}
				m_isVisible = true;
		}
		ImGui::End();
		ImGui::PopStyleVar();
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		//if(ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar))
		//{
		

		//	if (InspectorPanel::GetInstance().GetType() == InspectorPanel::SelectedType::GameObject)
		//	{

		//		GameObject entity = std::any_cast<GameObject>(InspectorPanel::GetInstance().GetHandle());
		//		if (entity.IsNotNull())
		//		{
		//			GameObject gm = { entity,SceneRegistry::get_current_scene() };
		//			if (gm.HasComponent<MeshRendererComponent>())
		//			{
		//				Ref<Mesh> mesh = gm.GetComponent<MeshRendererComponent>().mesh;
		//				ImGui::Begin("Mesh Stats");
		//				auto transform = Transform(gm);

		//				auto dir = (transform.GetWorldPosition() - m_EditorCamera.GetPosition()).normalize();
		//				auto cameraForward = m_EditorCamera.GetForward();

		//				auto trans = gm.GetComponent<TransformComponent>();

		//				auto proj = m_EditorCamera.GetViewProjection();

		//				float alpha = std::acos(Vector3::Dot(cameraForward, dir)) / (cameraForward.magnitude() * dir.magnitude());
		//				float C = Vector3::Distance(transform.GetWorldPosition(), m_EditorCamera.GetPosition());

		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				//float angle = std::acos(Vector3::Dot(camTrans.Position, trans.Position)) / (camTrans.Position.magnitude() * trans.Position.magnitude());

		//				ImGui::Text("Angle: %f", alpha);
		//				ImGui::Text("C: %f, B: %f, A: %f", C, B, A);
		//				ImGui::Text("Direction: %f %f %f", dir.x, dir.y, dir.z);
		//				ImGui::Text("Forward: %f %f %f", cameraForward.x, cameraForward.y, cameraForward.z);

		//				uint32_t indeciesCount = mesh->getIndexCount();

		//				ImGui::Text("Vertex Count: %d", mesh->getVertexCount());
		//				ImGui::Text("Index Count: %d", indeciesCount);

		//				ImGui::Text(mesh->getIndexBuffer()->m_DataStr.c_str());

		//				ImGui::End();
		//			}
		//		}
		//	}

		//	static bool pressed = false;
		//	static ImVec2 origin;
		//	static TransformComponent originTransform;
		//	static TransformComponent originWorldTransform;
		//	static GameObject originEntity;

		//	if (Helios::InputManager::IsKeyPressed(HL_KEY_MOUSE_LEFT) && ImGui::IsWindowHovered())
		//	//if (Helios::InputManager::IsKeyPressed(HL_KEY_MOUSE_LEFT) && ImGui::IsWindowFocused())
		//	{
		//		// Check if in bounds
		//		if (current_tool == GizmosRenderer::Tool::None && x >= 0 && y >= 0 && x < m_Framebuffer->GetWidth() && y < m_Framebuffer->GetHeight())
		//		{

		//			Color color = m_Framebuffer->GetPixel(0u, x, y);
		//			Color entId = m_Framebuffer->GetPixel(1u, x, y);
		//			int64_t id = std::round(entId.r);

		//			/*DepricatedApplication::ShowMessage("Pixel Clicked:",
		//					"X: " + std::to_string(x) +
		//					" Y: " + std::to_string(y) + "\n"
		//					"ID: " + std::to_string(entId.r) + "\n" +
		//					"R: " + std::to_string(color.r) +
		//					" G: " + std::to_string(color.g) +
		//					" B: " + std::to_string(color.b) +
		//					" A: " + std::to_string(color.a)
		//				);*/

		//			GameObject gm = SceneRegistry::GetPrimaryCamera();
		//			auto [trans, cam] = gm.GetComponent<TransformComponent, CameraComponent>();



		//			/*auto vec = editorCamera_old.ScreenToWorldCoordinates(x, y, 5.0f, m_Framebuffer->GetSize());
		//				std::cout << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z << std::endl;*/
		//			

		//			if (id == -1) InspectorPanel::GetInstance().Reset();
		//			else if (id > -1)
		//			{
		//				entt::entity entity = (entt::entity)id;

		//				/*if (SceneRegistry::get_current_scene()->m_components.valid(entity))
		//					InspectorPanel::GetInstance() << GameObject{ (entt::entity)entity, SceneRegistry::get_current_scene().get() };*/
		//			}
		//			else if (id >= -13 || current_tool != GizmosRenderer::Tool::None)
		//			{
		//				if (static_cast<HeliosEditor&>(HeliosEditor::GetInstance()).inspector.GetType() == InspectorPanel::SelectedType::GameObject)
		//				{
		//					GameObject obj = std::any_cast<GameObject>(InspectorPanel::GetInstance().GetHandle());
		//					//TransformComponent& transform = obj.GetComponent<TransformComponent>();
		//					Transform transform = Transform(obj);

		//					if (!pressed)
		//					{
		//						origin = ImGui::GetMousePos();
		//						originTransform = transform.GetTransformComponent();
		//						originWorldTransform = transform.GetWorldTransformCache();
		//						pressed = true;
		//						current_tool = (GizmosRenderer::Tool)id;
		//						originEntity = obj;
		//					}
		//				}
		//			}
		//		}
		//	}
		//	else { pressed = false; current_tool = GizmosRenderer::Tool::None; }

		//	if (current_tool != GizmosRenderer::Tool::None && originEntity.IsValid())
		//	{
		//		Transform transform(originEntity);
		//		//TransformComponent& transform = originEntity.GetComponent<TransformComponent>();

		//		switch (current_tool)
		//		{
		//		case GizmosRenderer::Tool::MoveX:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());

		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				//float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());

		//				float C = Vector3::Distance(originWorldTransform.Position, mvPoint);

		//				float a1 = C * std::sin(alpha);
		//				//float length = Vector3::Distance(originWorldTransform.Position, mvPoint * std::cos(alpha));
		//				//float length = std::cos(alpha) * Vector3::Distance(originWorldTransform.Position, mvPoint);

		//				std::cout << "Alpha: " << alpha << ", Cos: " << std::cos(alpha) << ", Length: " << a1 << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Right() * 1000.0f + originTransform.Position, originTransform.Right() * -1000.0f + originTransform.Position, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position + originTransform.Right() * a1 * (dir.x > 0.0f ? -1.0f : 1.0f));

		//				// DrawAngle mvPoint and originWorldTransform.Position
		//				// GizmosRenderer::DrawAngle(originWorldTransform.Position, mvPoint, dir, 5.0f, Color::Red, Matrix4x4::Identity());

		//				tool_lines.emplace(Line{ originWorldTransform.Position, originWorldTransform.Position + forw * a1, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Solid });
		//				tool_lines.emplace(Line{ originWorldTransform.Position, originWorldTransform.Position + originWorldTransform.Right() * a1, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Solid });
		//				tool_lines.emplace(Line{ originWorldTransform.Position, originWorldTransform.Position + dir * C, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });
		//				tool_lines.emplace(Line{ originWorldTransform.Position, mvPoint, Color::Yellow, 0.5f, -1, GizmosRenderer::LineMode::Rounded_Dash_Dot_Dot });
		//				tool_lines.emplace(Line{ originWorldTransform.Position + originWorldTransform.Right() * a1 * (alpha > 0.0f ? -1.0f : 1.0f), mvPoint, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Dash_Dot });

		//				/*auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Right() * 1000.0f + originTransform.Position, originTransform.Right() * -1000.0f + originTransform.Position, Color::Red, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position + originTransform.Right() * A * (dir.x > 0.0f ? -1.0f : 1.0f));*/
		//			}
		//			break;
		//		case GizmosRenderer::Tool::MoveZ:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Forward() * 1000.0f + originTransform.Position, originTransform.Forward() * -1000.0f + originTransform.Position, Color::Green, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position - originTransform.Forward() * A * (dir.x > 0.0f ? -1.0f : 1.0f));
		//			}
		//			break;
		//		case GizmosRenderer::Tool::MoveY:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float palpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float pC = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float pA = pC * std::sin(palpha);
		//				float pB = std::sqrt(std::pow(pC, 2.0f) - std::pow(pA, 2.0f));

		//				Vector3 mvPoint = m_EditorCamera.ScreenToWorldPoint(x, y, pB);
		//				auto forw = originWorldTransform.Forward();
		//				auto dir = (originWorldTransform.Position - mvPoint).normalize();
		//				float C = Vector3::Distance(mvPoint, originWorldTransform.Position);

		//				float alpha = std::acos(Vector3::Dot(forw, dir)) / (forw.magnitude() * dir.magnitude());


		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				std::cout << "Alpha: " << alpha << ", A: " << A << ", B: " << B << ", C: " << C << std::endl;

		//				tool_lines.emplace(Line{ originTransform.Up() * 1000.0f + originTransform.Position, originTransform.Up() * -1000.0f + originTransform.Position, Color::Blue, 0.5f, -1, GizmosRenderer::LineMode::Solid });

		//				transform.SetLocalPosition(originTransform.Position + originTransform.Up() * A * (dir.x > 0.0f ? -1.0f : 1.0f));
		//			}
		//			break;
		//		case GizmosRenderer::Tool::MoveXYZ:
		//			{
		//				auto cameraForward = m_EditorCamera.GetForward();
		//				auto direction = (originWorldTransform.Position - m_EditorCamera.GetPosition()).normalize();

		//				float alpha = std::acos(Vector3::Dot(cameraForward, direction)) / (cameraForward.magnitude() * direction.magnitude());
		//				float C = Vector3::Distance(originWorldTransform.Position, m_EditorCamera.GetPosition());

		//				float A = C * std::sin(alpha);
		//				float B = std::sqrt(std::pow(C, 2.0f) - std::pow(A, 2.0f));

		//				auto newLocation = m_EditorCamera.ScreenToWorldPoint(x, y, B);
		//				transform.SetLocalPosition(newLocation);
		//				tool_lines.emplace(Line{ originTransform.Position, newLocation, Color::BlanchedAlmond, 0.8f, -1, GizmosRenderer::LineMode::Rounded_Dashed });
		//				break;
		//			}
		//		default: break;
		//		}

		//		if (InputManager::IsKeyPressed(HL_KEY_ESCAPE)) { current_tool = GizmosRenderer::Tool::None; transform.GetTransformComponent() = originTransform; }

		//	}

		//	//static bool notf = true;

		//	//if (!SceneRegistry::GetCurrentScene().lock()->IsPrimaryCameraSet()) {
		//	//	if (notf) {
		//	//		auto pos = ImGui::GetWindowPos();
		//	//		auto Size = ImGui::GetWindowSize();
		//	//		ImGui::SetNextWindowFocus();
		//	//		ImGui::SetNextWindowBgAlpha(1.0f);
		//	//		ImGui::SetWindowFontScale(2);
		//	//		ImGui::SetNextWindowPos(ImVec2(pos.x + (Size.x / 2.0f) - 200.0f, pos.y + (Size.y / 2.0f) - 100.0f));
		//	//		ImGui::BeginChild("MissingCameraText", ImVec2(400, 200), true);
		//	//		//ImGui::Button("Test");
		//	//		//ImGui::SetCursorPos(ImVec2(0, 0));
		//	//		ImGui::Text("No CameraComponent Present for rendering!");
		//	//		if (ImGui::Button("Close")) {
		//	//			notf = false;
		//	//		}
		//	//		ImGui::EndChild();
		//	//	}
		//	//}
		//	//else notf = true;

		//	if (ImGui::IsWindowHovered())
		//	{
		//		static ImVec2 origin;
		//		static bool mActive = false;

		//		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		//		{
		//			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		//			origin = ImGui::GetMousePos();
		//			mActive = true;
		//		}

		//		if (mActive)
		//		{
		//			//std::cout << "Mouse Is Held" << std::endl;
		//			ImVec2 current = ImGui::GetMousePos();

		//			auto difference = Vector2(origin.x, origin.y) - Vector2(current.x, current.y);
		//			auto normalizedCordinates = -(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize();
		//			
		//			m_EditorCamera.HandleMovement(-(Vector2(origin.y, origin.x) - Vector2(current.y, current.x)).normalize());

		//			SetCursorPos(origin.x, origin.y);
		//		}

		//		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		//			mActive = false;
		//			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		//		}
		//	}

		//	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_R)) {
		//		m_EditorCamera.Reset();
		//	}
		//	m_isVisible = true;
		//		
		//}
		//else m_isVisible = false;

		//ImGui::End();
		//ImGui::PopStyleVar();
	}
}
