#include "HierarchyPanel.h"

#include <imgui.h>
#include <entt.hpp>
#include <Helios/Scene/SceneRegistry.h>

#include "BasePanel.h"
#include "InspectorPanel.h"
#include "AssetRegistry.h"

namespace Helios {

	void Popup(Entity entity) {
		Scene* scene = entity.GetScene();
		if (scene == nullptr) return;

		if (ImGui::MenuItem("Create Empty")) {
			if (entity.IsNotNull())
				InspectorPanel::GetInstance() << scene->InstantiateObject(entity.GetHandle());
			else
				InspectorPanel::GetInstance() << scene->InstantiateObject();
		}

		if (ImGui::MenuItem("Create PrimaryCamera")) {
			
			scene->CreateMainCamera({});
		}

		if (ImGui::MenuItem("Create Camera")) {
			scene->CreateCamera({});
		}

		if (ImGui::BeginMenu("3D Objects"))
		{
			if (ImGui::MenuItem("Cube")) {
				auto obj = scene->InstantiateObject("Cube");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetCubeMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << obj;
			}
			if(ImGui::MenuItem("Arrow"))
			{
				MeshBuilder arrowBuilder = Mesh::CreateGizmosArrow();

				auto obj = scene->InstantiateObject("Arrow");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::Create("Arrow", arrowBuilder);
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << obj;
			}
			if(ImGui::MenuItem("Plane"))
			{
				auto obj = scene->InstantiateObject("Plane");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetPlaneMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << obj;
			}
			if (ImGui::MenuItem("Cylinder"))
			{
				auto obj = scene->InstantiateObject("Cylinder");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetCylinderMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << obj;
			}
			if (ImGui::MenuItem("Cone"))
			{
				auto obj = scene->InstantiateObject("Cone");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetConeMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << obj;
			}
			if (ImGui::MenuItem("Sphere"))
			{
				auto obj = scene->InstantiateObject("Sphere");
				auto& meshRenderer = obj.AddComponent<MeshRendererComponent>();
				meshRenderer.mesh = Mesh::GetSphereMesh();
				meshRenderer.material = Material::Create(Material::Filter::MinMagPoint, Material::Type::Warp);
				InspectorPanel::GetInstance() << obj;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Lights"))
		{
			if (ImGui::MenuItem("Directional Light"))
			{
				auto obj = scene->InstantiateObject("Directional Light");
				obj.AddComponent<DirectionalLightComponent>(Color::White, 1.0f);
				InspectorPanel::GetInstance() << obj;
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Create Object With Sprite")) {
			auto obj = scene->InstantiateObject("Sprite");
			obj.AddComponent<SpriteRendererComponent>();
			InspectorPanel::GetInstance() << obj;
		}
		
	}

	void DrawObject(Entity object)
	{
		//ImGui::PushStyleColor(ImGuiCol_FrameBgActive)
		InfoComponent info = object.GetComponent<InfoComponent>();
		entt::entity child = entt::null;

		if (object.HasComponent<RelationshipComponent>())
		{
			auto& trans = object.GetComponent<RelationshipComponent>();
			if (trans.first_child != entt::null) child = trans.first_child;
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		if (child == entt::null) flags |= ImGuiTreeNodeFlags_Leaf;
		if (InspectorPanel::GetInstance().GetType() == InspectorPanel::SelectedType::Entity && std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle()).GetHandle() == object.GetHandle())
			flags |= ImGuiTreeNodeFlags_Selected;

		if (flags & ImGuiTreeNodeFlags_Selected) ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(15, 15, 15, 255));
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)object.GetHandle(), flags, info.name.c_str());

		if (ImGui::IsItemClicked())
		{
			InspectorPanel::GetInstance() << object;
			ImGui::SetWindowFocus("Inspector###inspector_main");
		}

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
				//GameObject other = { (*(entt::entity*)payload->Data), SceneRegistry::get_current_scene() };
				//if (other != object) {
				//	other.SetParent(object);
				//	//other.ResetParent();
				//}
				//ImGui::EndDragDropTarget();
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
				Entity tmp_entity = { child, object.GetScene() };
				DrawObject(tmp_entity);
				child = tmp_entity.GetComponent<RelationshipComponent>().next_sibling;
			}
			ImGui::TreePop();
		}
		

		if (flags & ImGuiTreeNodeFlags_Selected) ImGui::PopStyleColor();
		if(deleted) object.Destroy();
	}

	void HierarchyPanel::OnUpdate()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		if(ImGui::Begin("Hierarchy", &m_window_open))
		{
			// Change treenode indent for children of root
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 });
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5.f);
			int i = 0;
			for (auto& scene : SceneRegistry::GetActiveScenes())
			{
				ImGui::PushID(i);

				// bg color of first treenode = redish mixed with bg
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
				bool opened = ImGui::TreeNodeEx("scene", flags, scene->GetName().c_str());
				ImGui::PopStyleColor();

				if (ImGui::BeginPopupContextItem() || ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight))
				{
					Popup({ entt::null, scene });
					ImGui::EndPopup();
				}

				if (opened) {
					scene->m_components.each([&](auto entity)
					{
						Entity object{ entity, scene };
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
					ImGui::TreePop();
				}
				ImGui::PopID();
				i++;
			}
			ImGui::PopStyleVar(2);

			if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyDown(ImGuiKey_D))
			//if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_D))
			{
				if (InspectorPanel::GetInstance().GetType() == InspectorPanel::SelectedType::Entity)
				{
					Entity entity = std::any_cast<Entity>(InspectorPanel::GetInstance().GetHandle());

					if (entity.IsValid())
					{
						//entity.GetScene()->DuplicateEntity(entity);
						for (int i = 0; i < 50; i++)
						{
							entity.GetScene()->DuplicateEntity(entity);
						}
					}
				}
			}

			ImGui::End();
		}
		ImGui::PopStyleVar();

		

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

		/*const WeakRef<Scene>& scene = SceneRegistry::get_current_scene();

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
		});*/

		
	}
}