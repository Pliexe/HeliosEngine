#include "HierarchyPanel.h"

#include <imgui.h>
#include <entt.hpp>
#include <Helios/Scene/SceneRegistry.h>

#include "BasePanel.h"
#include "InspectorPanel.h"
#include "AssetRegistry.h"

namespace Helios {

	void Popup(entt::entity item = entt::null) {
		if (ImGui::MenuItem("Create Empty")) {
			if (item != entt::null)
				InspectorPanel::GetInstance() << SceneRegistry::get_current_scene()->InstantiateObject(item);
			else
				InspectorPanel::GetInstance() << SceneRegistry::get_current_scene()->InstantiateObject();
		}

		if (ImGui::MenuItem("Create PrimaryCamera")) {
			SceneRegistry::get_current_scene()->CreateMainCamera({});
		}

		if (ImGui::MenuItem("Create Camera")) {
			SceneRegistry::get_current_scene()->CreateCamera({});
		}

		if (ImGui::BeginMenu("3D Objects"))
		{
			if (ImGui::MenuItem("Cube")) {
				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Cube");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetCubeMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			if(ImGui::MenuItem("Arrow"))
			{
				MeshBuilder arrowBuilder = Mesh::CreateGizmosArrow();

				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Arrow");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::Create("Arrow", arrowBuilder);
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			if(ImGui::MenuItem("Plane"))
			{
				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Plane");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetPlaneMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Cylinder");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetCylinderMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			if (ImGui::MenuItem("Cone"))
			{
				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Cone");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetConeMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			if (ImGui::MenuItem("Sphere"))
			{
				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Sphere");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetSphereMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Lights"))
		{
			if (ImGui::MenuItem("Directional Light"))
			{
				auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Directional Light");
				obj.AddComponent<DirectionalLightComponent>(Color::White, 1.0f);
				InspectorPanel::GetInstance() << (entt::entity)obj;
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Create Object With Sprite")) {
			auto obj = SceneRegistry::get_current_scene()->InstantiateObject("Sprite");
			obj.AddComponent<SpriteRendererComponent>();
			InspectorPanel::GetInstance() << (entt::entity)obj;
		}
		
	}

	void DrawObject(GameObject object)
	{
		InfoComponent info = object.GetComponent<InfoComponent>();
		entt::entity child = entt::null;

		if (object.HasComponent<RelationshipComponent>())
		{
			auto& trans = object.GetComponent<RelationshipComponent>();
			if (trans.first_child != entt::null) child = trans.first_child;
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (child == entt::null) flags |= ImGuiTreeNodeFlags_Leaf;
		if (InspectorPanel::GetInstance() == object) flags |= ImGuiTreeNodeFlags_Selected;

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
					if (object.HasComponent<SpriteRendererComponent>())
					{
						object.GetComponent<SpriteRendererComponent>().texture = texture;
					}
					else if (object.HasComponent<MeshRendererComponent>())
					{
						object.GetComponent<MeshRendererComponent>().material.get()->texture = texture;
					}
				}
				ImGui::EndDragDropTarget();
			} else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_HIERARCHY_ITEM", 0))
			{
				GameObject other = { (*(entt::entity*)payload->Data), SceneRegistry::get_current_scene() };
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
				DrawObject({child,SceneRegistry::get_current_scene()});
				child = GameObject(child, SceneRegistry::get_current_scene()).GetComponent<RelationshipComponent>().next_sibling;
			}
			ImGui::TreePop();
		}
		

		if(deleted) object.Destroy();
	}

	void HierarchyPanel::OnUpdate()
	{
		// Changed
		//if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight, false))
		if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight))
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

		const WeakRef<Scene>& scene = SceneRegistry::get_current_scene();

		int i = 0;
		scene.lock()->m_components.each([&](auto entity)
		{
			GameObject object{ entity, SceneRegistry::get_current_scene() };
			if (object.HasComponent<RelationshipComponent>())
			{
				auto& relation = object.GetComponent<RelationshipComponent>();
				if (relation.HasParent()) return;
			}
			ImGui::PushID(i);
			DrawObject(object);
			ImGui::PopID();
			i++;
		});

		
	}
}