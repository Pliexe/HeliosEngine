#include "InspectorPanel.h"

#include <imgui.h>

#include <Helios/GameObjects/Components/Camera.h>
#include <Helios/Resources/Brushes/Brush.h>
#include <Helios/Resources/Brushes/SolidBrush.h>
#include <Helios/GameObjects/Components/RectangleComponent.h>
#include <Helios/Scene/Components.h>


namespace Helios {

	InspectorPanel* InspectorPanel::instance = nullptr;

	template <typename T> void AddComponentItem(const char* name, GameObject& obj) { if (!obj.HasComponent<T>() && ImGui::Button(name)) obj.AddComponent<T>(); }

	void InspectorPanel::OnUpdate() {
		switch (type)
		{
		case SelectedType::GameObject:
		{
			GameObject gm = std::any_cast<entt::entity>(handle);

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

			if (gm.HasComponent<Components::Transform2D>())
			{

				if (ImGui::CollapsingHeader("Transform2D")) {
					Components::Transform2D& transform = gm.GetComponent<Components::Transform2D>();
					float* vec2Pos = transform.position;
					ImGui::DragFloat2("Position", vec2Pos);
					transform.position = { vec2Pos[0], vec2Pos[1] };

					float* vec2Size = transform.size;
					ImGui::DragFloat2("Size", vec2Size);
					transform.size = { vec2Size[0], vec2Size[1] };
				}
			}

			if (gm.HasComponent<Components::Camera>())
			{
				if (ImGui::CollapsingHeader("Camera")) {
					ImGui::ColorEdit4("Clear Color", gm.GetComponent<Components::Camera>().clear_color);
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

			pos = ImGui::GetCursorPos();
			ImGui::Text("Test");

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