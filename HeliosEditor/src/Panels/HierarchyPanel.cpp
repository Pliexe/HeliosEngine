#include "BasePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"

#include <Helios/Scene/SceneManager.h>
#include "AssetRegistry.h"
#include "imgui.h"
#include "entt.hpp"

namespace Helios {

	void Popup(entt::entity item = entt::null) {
		if (ImGui::MenuItem("Create Empty")) {
			if (item != entt::null)
			{
				GameObject obj = GameObject(item);
				GameObject::InstantiateObject(obj);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			else
				InspectorPanel::GetInstance() << (entt::entity)GameObject::InstantiateObject();
		}

		if (ImGui::MenuItem("Create PrimaryCamera")) {
			GameObject::CreateMainCamera();
		}

		if (ImGui::MenuItem("Create Camera")) {
			GameObject::CreateCamera();
		}

		if (ImGui::BeginMenu("3D Objects"))
		{
			if (ImGui::MenuItem("Cube")) {
				auto obj = GameObject::InstantiateObject("Cube");
				auto& meshRenderer = obj.AddComponent<Components::MeshRenderer>();
				meshRenderer.mesh = Mesh::GetCubeMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Create Object With Sprite")) {
			auto obj = GameObject::InstantiateObject("Sprite");
			obj.AddComponent<Components::SpriteRenderer>();
			InspectorPanel::GetInstance() << (entt::entity)obj;
		}
		
	}

	void DrawObject(GameObject object)
	{
		Components::InfoComponent info = object.GetComponent<Components::InfoComponent>();
		entt::entity child = entt::null;

		if (object.HasComponent<Components::Relationship>())
		{
			auto& trans = object.GetComponent<Components::Relationship>();
			if (trans.first_child != entt::null) child = trans.first_child;
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (child == entt::null) flags |= ImGuiTreeNodeFlags_Leaf;
		if (InspectorPanel::GetInstance() == (entt::entity)object) flags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entt::entity)object, flags, info.name.c_str());

		if (ImGui::IsItemClicked())
			InspectorPanel::GetInstance() << (entt::entity)object;

		bool deleted = false;
		
		if (ImGui::BeginPopupContextItem())
		{
			Popup(object);

			ImGui::Separator();

			if (ImGui::MenuItem("Delete"))
				deleted = true;

			if (ImGui::MenuItem("Rename"))
				InspectorPanel::GetInstance().FocusRename(object);
				
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags target_flags = 0;
			//target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
			//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
			
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_EXPLORER_TEXTURE2D", target_flags))
			{
				const char* key = (const char*)payload->Data;
				Ref<Texture2D> texture = Helios::AssetRegistry::GetTexture(key);
				if (texture) {
					if (object.HasComponent<Components::SpriteRenderer>())
					{
						object.GetComponent<Components::SpriteRenderer>().texture = texture;
					}
					else if (object.HasComponent<Components::MeshRenderer>())
					{
						object.GetComponent<Components::MeshRenderer>().material.get()->texture = texture;
					}
				}
				ImGui::EndDragDropTarget();
			} else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_ITEM", 0))
			{
				GameObject other = (*(entt::entity*)payload->Data);
				if (other != object) {
					other.SetParent(object);
					//other.ResetParent();
				}
				ImGui::EndDragDropTarget();
			}
			
		}

		ImGuiDragDropFlags src_flags = 0;
		src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
		src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
		if (ImGui::BeginDragDropSource(src_flags))
		{
			if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
				ImGui::Text(object.GetName().c_str());
			entt::entity payload = object;
			ImGui::SetDragDropPayload("DND_HIERARCHY_ITEM", &payload, sizeof(entt::entity));
			ImGui::EndDragDropSource();
		}

		if (opened) {
			while (child != entt::null)
			{
				DrawObject(child);
				child = GameObject(child).GetComponent<Components::Relationship>().next_child;
			}
			ImGui::TreePop();
		}
		

		if(deleted) GameObject::DestroyObject(object);
	}

	void HierarchyPanel::OnUpdate()
	{
		if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight, false))
		{
			Popup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags target_flags = 0;
			//target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
			//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_ITEM", target_flags))
			{
				//GameObject(*(entt::entity*)payload->Data).ResetParent();
			}
			ImGui::EndDragDropTarget();
		}

		const WeakRef<Scene>& scene = SceneManager::GetCurrentScene();

		int i = 0;
		scene.lock()->m_components.each([&](auto entity)
		{
			GameObject object{ entity };
			if (object.HasComponent<Components::Relationship>())
			{
				auto& relation = object.GetComponent<Components::Relationship>();
				if (relation.HasParent()) return;
			}
			ImGui::PushID(i);
			DrawObject(object);
			ImGui::PopID();
			i++;
		});

		
	}
}