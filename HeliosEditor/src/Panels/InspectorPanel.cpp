#include "InspectorPanel.h"

#include <imgui.h>

#include "../AssetRegistry.h"
#include "Helios/Scene/Components/Transforms.h"
#include "ImGuiCustomControls.h"
#include <Helios/Scene/Components.h>


namespace Helios
{

InspectorPanel* InspectorPanel::instance = nullptr;

template <typename T> void AddComponentItem(const char* name, Entity& obj)
{
    if (!obj.HasComponent<T>() && ImGui::Button(name))
        obj.AddScopedComponent<T>();
}

void InspectorPanel::OnUpdate()
{
    switch (type)
    {
    case SelectedType::Entity:
    {
        Entity entity = std::any_cast<Entity>(handle);
        if (entity.IsNull())
            return;
        if (!entity.IsValid())
        {
            handle = nullptr;
            return;
        }

        bool enabled = !entity.HasComponent<DisabledObjectComponent>();
        if (ImGui::Checkbox("##gm_enabled", &enabled))
        {
            if (enabled)
                entity.RemoveComponent<DisabledObjectComponent>();
            else
                entity.AddScopedComponent<DisabledObjectComponent>();
        }

        /*ImGui::Checkbox("##active", &gm.active);
        ImGui::SameLine();*/

        char name[100];
#ifdef HELIOS_PLATFORM_WINDOWS
        strcpy_s(name, entity.GetName().c_str());
#else
        strcpy(name, entity.GetName().c_str());
#endif
        if (focus_next_name_input)
        {
            ImGui::SetKeyboardFocusHere(0);
            focus_next_name_input = false;
        }
        if (ImGui::InputText("##gm_name", name, 100,
                             ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (name[0] != '\0')
                entity.GetName() = name;
        }

        ImGui::Spacing();

        if (entity.HasComponent<Transform2DComponent>())
        {

            if (ImGui::CollapsingHeader("Transform2DComponent"))
            {
                ImGui::Text("X:");
                ImGui::SameLine();
                Transform2DComponent& transform =
                    entity.GetComponent<Transform2DComponent>();
                float* vec2Pos = transform.position;
                ImGui::DragFloat2("Position", vec2Pos);
                transform.position = {vec2Pos[0], vec2Pos[1]};

                ImGui::Text("Y:");
                ImGui::SameLine();
                float* vec2Size = transform.size;
                ImGui::DragFloat2("Size", vec2Size);
                transform.size = {vec2Size[0], vec2Size[1]};
            }
        }

        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::CollapsingHeader("Transform",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto transform = Transform(entity);
                TransformComponent& comp = transform.GetTransformComponent();
                ImVec2 size = ImGui::GetContentRegionAvail();

                if (ImGui::EditVector3("Position", size.x, comp.Position))
                {
                    comp.changed = true;
                }
                ImGui::EditQuanterionEuler("Rotation", size.x, comp.Rotation);
                ImGui::EditVector3("Scale", size.x, comp.Scale);

                entity.GetComponent<TransformComponent>().SetLocalTransform(comp);

                // ImGui::Checkbox("Switch implementation: ",
                // &transform.typeSwitch);

            }
        }

        if (entity.HasComponent<CameraComponent>())
        {
            if (ImGui::CollapsingHeader("Camera",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& cam = entity.GetComponent<CameraComponent>();

                int cmSelected = static_cast<int>(cam.background_mode);
                static const char* cmItems[] = {"None", "Solid Color",
                                                "Skybox"};
                if (ImGui::Combo("Clear Mode: ", &cmSelected, cmItems,
                                 IM_ARRAYSIZE(cmItems)))
                {
                    cam.background_mode =
                        (CameraComponent::BackgroundMode)cmSelected;
                }

                ImGui::ColorEdit4("Clear Color", cam.clear_color);

                /*if
                (!SceneRegistry::get_current_scene()->IsPrimaryCamera(entity) &&
                ImGui::Button("Make Primary")) {
                        SceneRegistry::get_current_scene()->SetPrimaryCamera(entity);
                }*/

                ImGui::Checkbox("Is Primary", &cam.isPrimary);

                int selected = cam.ortographic;
                static const char* items[] = {"Perspective", "Ortographic"};
                if (ImGui::Combo("Perspective: ", &selected, items, 2))
                {
                    cam.ortographic = selected;
                }

                if (cam.ortographic)
                {
                    ImGui::DragFloat("Size", &cam.size, 0.2f);
                }
                else
                {
                    ImGui::SliderFloat("Fov", &cam.fov, 1e-04f, 180.0f);
                }

                ImGui::Text("Clipping Planes: ");
                ImGui::DragFloat("Near", &cam.near_z);
                ImGui::DragFloat("Far", &cam.far_z);
            }
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            if (ImGui::CollapsingHeader("Sprite Renderer",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& sRenderer =
                    entity.GetComponent<SpriteRendererComponent>();

                ImGui::ColorEdit4("Color", sRenderer.color);

                if (sRenderer.texture == nullptr)
                    ImGui::Text("Texture: None");
                else
                    ImGui::Text("Texture: Unknown Name!");

                if (ImGui::Button("Select"))
                {
                    Entity selectedGm = entity;
                    AssetRegistry::OpenTextureSelect(
                        [selectedGm](Ref<Texture2D> nTexture) mutable
                        {
                            auto& renderer =
                                selectedGm
                                    .GetComponent<SpriteRendererComponent>()
                                    .texture = nTexture;
                        });
                }
            }
        }

        if (entity.HasComponent<MeshRendererComponent>())
        {
            if (ImGui::CollapsingHeader("Mesh Renderer",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& sRenderer = entity.GetComponent<MeshRendererComponent>();

                ImGui::Checkbox("Cast Shadows", &sRenderer.castShadow);

                ImGui::ColorEdit4("Color", sRenderer.material->AlbedoColor);

                if (sRenderer.material->texture == nullptr)
                {
                    ImGui::Text("Texture: None");
                }
                else
                {
                    ImGui::Text("Texture: Unknown Name!");
                    ImGui::Image(sRenderer.material->texture->GetTextureID(),
                                 ImVec2(64, 64));
                }

                ImGui::Text("Mesh UUID: %s",
                            sRenderer.mesh == nullptr
                                ? "None"
                                : sRenderer.mesh->GetID().toString().c_str());

                if (ImGui::Button("Select"))
                {
                    Entity selectedGm = entity;
                    AssetRegistry::OpenTextureSelect(
                        [selectedGm](Ref<Texture2D> nTexture) mutable
                        {
                            auto& renderer =
                                selectedGm.GetComponent<MeshRendererComponent>()
                                    .material->texture = nTexture;
                        });
                }

                if (sRenderer.mesh != nullptr)
                {
                    ImGui::Text("Vertecies: %d",
                                sRenderer.mesh->GetVertexCount());
                    ImGui::Text("Indecies: %d",
                                sRenderer.mesh->GetIndexCount());
                }
            }
        }

        if (entity.HasComponent<DirectionalLightComponent>())
        {
            if (ImGui::CollapsingHeader("Directional Light",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& light = entity.GetComponent<DirectionalLightComponent>();

                ImGui::ColorEdit4("Color", light.color);
                ImGui::DragFloat("Intensity", &light.intensity, 0.1f, 0.0f,
                                 100.0f);
            }
        }

        if (entity.HasComponent<Rigidbody2D>())
        {
            if (ImGui::CollapsingHeader("Rigidbody2DComponent",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& rigidbody = entity.GetComponent<Rigidbody2D>();

                // rigidbody.type enum { Static, Dynamic, Kinematic }; make a
                // dropdown to select type
                if (ImGui::BeginCombo(
                        "Type", rigidbody.type == BodyType::Static ? "Static"
                                : rigidbody.type == BodyType::Dynamic
                                    ? "Dynamic"
                                    : "Kinematic"))
                {
                    if (ImGui::Selectable("Static",
                                          rigidbody.type == BodyType::Static))
                        rigidbody.type = BodyType::Static;
                    if (ImGui::Selectable("Dynamic",
                                          rigidbody.type == BodyType::Dynamic))
                        rigidbody.type = BodyType::Dynamic;
                    if (ImGui::Selectable("Kinematic", rigidbody.type ==
                                                           BodyType::Kinematic))
                        rigidbody.type = BodyType::Kinematic;

                    ImGui::EndCombo();
                }

                ImGui::Checkbox("Is Kinematic", &rigidbody.isKinematic);
                ImGui::Checkbox("Is Gravity", &rigidbody.useGravity);

                ImGui::DragFloat("Mass", &rigidbody.mass, 0.1f, 0.0f, 100.0f);

                ImGui::EditVector2("Velocity", 0.0f, rigidbody.velocity);
            }
        }

        if (entity.HasComponent<BoxCollider2D>())
        {
            if (ImGui::CollapsingHeader("BoxCollider2D",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& collider = entity.GetComponent<BoxCollider2D>();

                ImGui::EditVector2("Size", 0.0f, collider.size);
                ImGui::EditVector2("Offset", 0.0f, collider.offset);

                ImGui::Text("Is body nullptr: %s",
                            collider.body == nullptr ? "true" : "false");
            }
        }

        if (entity.HasComponent<CircleCollider2D>())
        {
            if (ImGui::CollapsingHeader("CircleCollider2D",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto& collider = entity.GetComponent<CircleCollider2D>();

                ImGui::DragFloat("Radius", &collider.radius, 0.1f, 0.0f,
                                 100.0f);
                ImGui::EditVector2("Offset", 0.0f, collider.offset);
            }
        }

        for (auto& service_name : Scripting::GetRegisteredServices())
        {
            if (entity.HasBehaviours(service_name))
            {
                if (ImGui::CollapsingHeader(service_name.c_str(),
                                            ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto iter = entity.GetBehavioursIter(service_name);

                    std::vector<Scripting::Behaviour> behaviours(iter.first,
                                                                 iter.second);

                    for (auto& behaviour : behaviours)
                    {
                        if (ImGui::CollapsingHeader(
                                behaviour.GetName().c_str(),
                                ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            bool enabled = behaviour.IsEnabled();
                            if (ImGui::Checkbox("Enabled", &enabled))
                            {
                                Scripting::GetBehaviourStore(service_name)
                                    .SetEnabled(behaviour, enabled);
                            }
                            ImGui::Text("Behaviour Instance Address: %lu",
                                        behaviour.GetHandle());
                            ImGui::Text("Language: %s",
                                        behaviour.GetServiceName().c_str());
                            if (ImGui::Button("Remove"))
                            {
                                entity.RemoveBehaviour(service_name, behaviour);
                                break;
                            }
                        }
                    }

                    // auto behaviours = entity.GetBehaviours(service_name);

                    // for (auto &behaviour : behaviours)
                    // {
                    // 	if (ImGui::CollapsingHeader(behaviour.GetName().c_str(),
                    // ImGuiTreeNodeFlags_DefaultOpen))
                    // 	{
                    // 		bool enabled = behaviour.IsEnabled();
                    // 		if (ImGui::Checkbox("Enabled", &enabled)) {
                    // 			Scripting::GetBehaviourStore(service_name).SetEnabled(behaviour,
                    // enabled);
                    // 		}
                    // 		ImGui::Text("Behaviour Instance Address: %p",
                    // behaviour.GetHandle()); 		ImGui::Text("Language: %s",
                    // behaviour.GetServiceName().c_str()); 		if
                    // (ImGui::Button("Remove"))
                    // 		{
                    // 			entity.RemoveBehaviour(service_name,
                    // behaviour);
                    // 		}
                    // 	}
                    // }
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
            if (m_addBehaviourPopup)
            {
                for (const auto& service_name :
                     Scripting::GetRegisteredServices())
                {
                    for (const auto& behaviourName :
                         Scripting::GetService(service_name)
                             .GetBehaviourNames())
                    {
                        if (ImGui::Button(
                                (service_name + " : " + behaviourName).c_str()))
                        {
                            if (entity.IsValid())
                            {
                                entity.AddBehaviour(service_name,
                                                    behaviourName);
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }
                }
            }
            else
            {
                AddComponentItem<CameraComponent>("Camera", entity);
                AddComponentItem<SpriteRendererComponent>("SpriteRenderer",
                                                          entity);
                AddComponentItem<Rigidbody2D>("Rigidbody2D", entity);
                AddComponentItem<BoxCollider2D>("BoxCollider2D", entity);
                AddComponentItem<CircleCollider2D>("SphereCollider2D", entity);

                if (ImGui::Button("Add Behaviour"))
                {
                    m_addBehaviourPopup = true;
                }
            }

            ImGui::EndPopup();
        }
        else
            m_addBehaviourPopup = false;
    }
    break;
    default:
        break;
    }
}
} // namespace Helios
