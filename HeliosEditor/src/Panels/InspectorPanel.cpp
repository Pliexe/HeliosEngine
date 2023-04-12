#include "InspectorPanel.h"

#include <imgui.h>

#include <Helios/Resources/Brushes/Brush.h>
#include <Helios/Resources/Brushes/SolidBrush.h>
#include <Helios/GameObjects/Components/RectangleComponent.h>
#include <Helios/Scene/Components.h>
#include "../AssetRegistry.h"
#include "ImGuiCustomControls.h"
#include "Helios/Core/Application.h"

namespace Helios {

	InspectorPanel* InspectorPanel::instance = nullptr;

	template <typename T> void AddComponentItem(const char* name, GameObject& obj) { if (!obj.HasComponent<T>() && ImGui::Button(name)) obj.AddComponent<T>(); }

	void InspectorPanel::OnUpdate() {
		switch (type)
		{
		case SelectedType::GameObject:
		{
			GameObject entity = std::any_cast<GameObject>(handle);
			if (entity.IsNull()) return;
			if(!entity.IsValid())
			{
				handle = nullptr;
				return;
			}

			bool enabled = !entity.HasComponent<DisabledObjectComponent>();
			if(ImGui::Checkbox("##gm_enabled", &enabled))
			{
				if (enabled) entity.RemoveComponent<DisabledObjectComponent>();
				else entity.AddComponent <DisabledObjectComponent>();
			}
			
			/*ImGui::Checkbox("##active", &gm.active);
			ImGui::SameLine();*/

			char name[100];
			strcpy_s(name, entity.GetName().c_str());
			if (focus_next_name_input) {
				ImGui::SetKeyboardFocusHere(0);
				focus_next_name_input = false;
			}
			if (ImGui::InputText("##gm_name", name, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (name[0] != '\0')
					entity.GetName() = name;
			}

			ImGui::Spacing();

			if (entity.HasComponent<Transform2DComponent>())
			{

				if (ImGui::CollapsingHeader("Transform2DComponent")) {
					ImGui::Text("X:");
					ImGui::SameLine();
					Transform2DComponent& transform = entity.GetComponent<Transform2DComponent>();
					float* vec2Pos = transform.position;
					ImGui::DragFloat2("Position", vec2Pos);
					transform.position = { vec2Pos[0], vec2Pos[1] };

					ImGui::Text("Y:");
					ImGui::SameLine();
					float* vec2Size = transform.size;
					ImGui::DragFloat2("Size", vec2Size);
					transform.size = { vec2Size[0], vec2Size[1] };
				}
			}

			if (entity.HasComponent<TransformComponent>())
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto transform = Transform(entity);
					TransformComponent& comp = transform.GetTransformComponent();

					ImGui::EditVector3("Position", comp.Position);
					ImGui::EditQuanterionEuler("RotationRow Euler", comp.Rotation);
					ImGui::EditQuanterion("RotationRow", comp.Rotation, 0.01, -1.0f, 1.0f);
					ImGui::EditVector3("Scale", comp.Scale);

					//transform.SetLocalTransform(comp.Position, comp.RotationRow, comp.Scale);

					//ImGui::Checkbox("Switch implementation: ", &transform.typeSwitch);
				}
			}

			if (entity.HasComponent<CameraComponent>())
			{
				if (ImGui::CollapsingHeader("CameraComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& cam = entity.GetComponent<CameraComponent>();
					ImGui::ColorEdit4("Clear Color", cam.clear_color);

					/*if (!SceneRegistry::get_current_scene()->IsPrimaryCamera(entity) && ImGui::Button("Make Primary")) {
						SceneRegistry::get_current_scene()->SetPrimaryCamera(entity);
					}*/

					ImGui::Checkbox("Is Primary", &cam.isPrimary);

					int selected = cam.ortographic;
					static const char* items[] = { "Perspective", "Ortographic" };
					if (ImGui::Combo("Perspective: ", &selected, items, 2)) {
						cam.ortographic = selected;
					}

					if (cam.ortographic) {
						ImGui::DragFloat("Size", &cam.size, 0.2f);
					}
					else {
						ImGui::SliderFloat("Fov", &cam.fov, 1e-04f, 180.0f);
					}

					ImGui::Text("Clipping Planes: ");
					ImGui::DragFloat("Near", &cam.near_z);
					ImGui::DragFloat("Far", &cam.far_z);
				}
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::CollapsingHeader("SpriteRendererComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& sRenderer = entity.GetComponent<SpriteRendererComponent>();

					ImGui::ColorEdit4("Color", sRenderer.color);

					if (sRenderer.texture == nullptr)
						ImGui::Text("Texture: None");
					else
						ImGui::Text("Texture: Unknown Name!");

					if (ImGui::Button("Select")) {
						GameObject selectedGm = entity;
						AssetRegistry::OpenTextureSelect([selectedGm](Ref<Texture2D> nTexture) mutable {
							auto& renderer = selectedGm.GetComponent<SpriteRendererComponent>().texture = nTexture;
						});
					}
				}
			}

			if (entity.HasComponent<MeshRendererComponent>())
			{
				if (ImGui::CollapsingHeader("MeshRendererComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& sRenderer = entity.GetComponent<MeshRendererComponent>();

					ImGui::ColorEdit4("Color", sRenderer.material->Color);

					if (sRenderer.material->texture == nullptr)
						ImGui::Text("Texture: None");
					else
						ImGui::Text("Texture: Unknown Name!");

					if (ImGui::Button("Select")) {
						GameObject selectedGm = entity;
						AssetRegistry::OpenTextureSelect([selectedGm](Ref<Texture2D> nTexture) mutable {
							auto& renderer = selectedGm.GetComponent<MeshRendererComponent>().material->texture = nTexture;
							});
					}
				}
			}
			
			if (entity.HasComponent<DirectionalLightComponent>())
			{
				if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& light = entity.GetComponent<DirectionalLightComponent>();

					ImGui::ColorEdit4("Color", light.color);
					ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.0f, 100.0f);
				}
			}


			ImVec2 lastSize = ImGui::GetWindowSize();
			ImVec2 lastPos = ImGui::GetWindowPos();
			
			

			ImGui::Dummy(ImVec2(0.0f, 50.0f));
			ImGui::Spacing();
			ImVec2 pos;
			ImGui::SameLine((lastSize.x / 2) - 75.0f);
			if (ImGui::Button("Add Component", ImVec2(150.0f, 0.0f)))
			{
			ImGui::SetNextWindowSize(ImVec2(800.0f, 500.0f));
				ImGui::OpenPopup("Add Component");
			}

			pos.x += ImGui::GetWindowPos().x + (lastSize.x / 2) - 100.0f;
			pos.y += ImGui::GetWindowPos().y;

			ImGui::SetNextWindowPos(pos);
			if (ImGui::BeginPopupContextItem("Add Component"))
			{
				AddComponentItem<CameraComponent>("CameraComponent", entity);
				AddComponentItem<SpriteRendererComponent>("SpriteRendererComponent", entity);

				ImGui::EndPopup();
			}

		}
			break;
		default:
			break;
		}
	}
}
