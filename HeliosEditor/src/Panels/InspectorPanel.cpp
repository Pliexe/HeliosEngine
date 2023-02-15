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
			entt::entity entity = std::any_cast<entt::entity>(handle);
			if (entity == entt::null) return;
			GameObject gm = entity;

			bool enabled = !gm.HasComponent<Components::DisabledObject>();
			if(ImGui::Checkbox("##gm_enabled", &enabled))
			{
				if (enabled) gm.RemoveComponent<Components::DisabledObject>();
				else gm.AddComponent <Components::DisabledObject>();
			}
			
			/*ImGui::Checkbox("##active", &gm.active);
			ImGui::SameLine();*/

			char name[100];
			strcpy_s(name, gm.GetName().c_str());
			if (focus_next_name_input) {
				ImGui::SetKeyboardFocusHere(0);
				focus_next_name_input = false;
			}
			if (ImGui::InputText("##gm_name", name, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (name[0] != '\0')
					gm.GetName() = name;
			}

			ImGui::Spacing();

			if (gm.HasComponent<Components::Transform2D>())
			{

				if (ImGui::CollapsingHeader("Transform2D")) {
					ImGui::Text("X:");
					ImGui::SameLine();
					Components::Transform2D& transform = gm.GetComponent<Components::Transform2D>();
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

			if (gm.HasComponent<Components::Transform>())
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& transform = gm.GetComponent<Components::Transform>();

					ImGui::EditVector3("Position", transform.position);

					ImGui::EditQuanterionEuler("Rotation Euler", transform.rotation);
					ImGui::EditQuanterion("Rotation", transform.rotation, 0.01, -1.0f, 1.0f);

					//ImGui::EditVector3("Rotation", transform.rotationVec);

					ImGui::EditVector3("Scale", transform.scale);


					ImGui::Checkbox("Switch implementation: ", &transform.typeSwitch);
				}
			}

			if (gm.HasComponent<Components::Camera>())
			{
				if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& cam = gm.GetComponent<Components::Camera>();
					ImGui::ColorEdit4("Clear Color", cam.clear_color);

					if (!SceneManager::GetCurrentScene().lock()->IsPrimaryCamera(gm) && ImGui::Button("Make Primary")) {
						SceneManager::GetCurrentScene().lock()->SetPrimaryCamera(gm);
					}

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

			if (gm.HasComponent<Components::SpriteRenderer>())
			{
				if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& sRenderer = gm.GetComponent<Components::SpriteRenderer>();

					ImGui::ColorEdit4("Color", sRenderer.color);

					if (sRenderer.texture == nullptr)
						ImGui::Text("Texture: None");
					else
						ImGui::Text("Texture: Unknown Name!");

					if (ImGui::Button("Select")) {
						GameObject selectedGm = gm;
						AssetRegistry::OpenTextureSelect([selectedGm](Ref<Texture2D> nTexture) mutable {
							auto& renderer = selectedGm.GetComponent<Components::SpriteRenderer>().texture = nTexture;
						});
					}
				}
			}

			if (gm.HasComponent<Components::MeshRenderer>())
			{
				if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto& sRenderer = gm.GetComponent<Components::MeshRenderer>();

					ImGui::ColorEdit4("Color", sRenderer.material->Color);

					if (sRenderer.material->texture == nullptr)
						ImGui::Text("Texture: None");
					else
						ImGui::Text("Texture: Unknown Name!");

					if (ImGui::Button("Select")) {
						GameObject selectedGm = gm;
						AssetRegistry::OpenTextureSelect([selectedGm](Ref<Texture2D> nTexture) mutable {
							auto& renderer = selectedGm.GetComponent<Components::MeshRenderer>().material->texture = nTexture;
							});
					}
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
				AddComponentItem<Components::Camera>("Camera", gm);
				AddComponentItem<Components::SpriteRenderer>("SpriteRenderer", gm);

				ImGui::EndPopup();
			}

		}
			break;
		default:
			break;
		}
	}
}
