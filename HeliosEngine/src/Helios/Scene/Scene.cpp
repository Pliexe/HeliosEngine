/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Helios/Math/Vector.h"
#include "Helios/Scene/Entity.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/Renderer.h"
#include "Helios/Graphics/Renderer2D.h"
#include "Scene.h"

#include <execution>

#include "Helios/Core/Time.h"

#include "Components.h"
#include "EntityHelpers.h"
#include "Helios/Core/Application.h"
#include "Helios/Graphics/GizmosRenderer.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Physics/Physics2D.h"

namespace Helios {

	Scene::~Scene() {
		//DepricatedHelios::ShowMessage("Test","Scene Deleted!");
	}

	void Scene::UpdateChildTransforms(Ref<Scene> scene)
	{  
		/*auto view = scene->m_components.view<TransformComponent, RelationshipComponent>();
		for (auto entity : view)
		{
			auto [transform, relationship] = view.get<TransformComponent, RelationshipComponent>(entity);

			if (relationship.HasParent()) continue;
			
			static std::function<void(entt::entity, RelationshipComponent&, TransformComponent&, TransformComponent&)> UpdateTransform = [scene](entt::entity entity, RelationshipComponent& relationship, TransformComponent& transform, TransformComponent& parentTransform)
			{
				transform.position = transform.Position + parentTransform.Position;
				transform.rotation = transform.RotationRow * parentTransform.RotationRow;
				transform.scale = {
					transform.Scale.x * parentTransform.Scale.x,
					transform.Scale.y * parentTransform.Scale.y,
					transform.Scale.z * parentTransform.Scale.z
				};

				if (relationship.first_child != entt::null)
				{
					auto [childTransform, childRelationship] = scene->m_components.get<TransformComponent, RelationshipComponent>(relationship.first_child);
					UpdateTransform(relationship.first_child, childRelationship, childTransform, transform);
				}
				if(relationship.next_sibling != entt::null)
				{
					auto [childTransform, childRelationship] = scene->m_components.get<TransformComponent, RelationshipComponent>(relationship.next_sibling);
					UpdateTransform(relationship.next_sibling, childRelationship, childTransform, transform);
				}
				if(relationship.prev_sibling != entt::null)
				{
					auto [childTransform, childRelationship] = scene->m_components.get<TransformComponent, RelationshipComponent>(relationship.prev_sibling);
					UpdateTransform(relationship.prev_sibling, childRelationship, childTransform, transform);
				}
			}; 

			if (relationship.first_child != entt::null)
			{
				auto [childTransform, childRelationship] = scene->m_components.get<TransformComponent, RelationshipComponent>(relationship.first_child);
				UpdateTransform(relationship.first_child, childRelationship, childTransform, transform);
			}
		}*/
	}

	void Scene::RenderScene(Ref<Framebuffer>& colorBuffer, EditorCamera& camera)
	{
		Matrix4x4 projection = camera.GetViewProjection();
		RenderScene(colorBuffer, camera.GetTransformComponent(), projection);
		colorBuffer->Bind();
		RenderGizmos(projection);
		colorBuffer->Unbind();
	}

	void Scene::PerformCleanupAndSync()
	{
		for (auto entity : m_entitiesToDestroy)
		{
			if (m_components.any_of<BoxCollider2D>(entity))
			{
				auto& collider = m_components.get<BoxCollider2D>(entity);
			}
			m_components.destroy(entity);
		}
		m_entitiesToDestroy.clear();

		// sync physics

		for (auto entity : m_components.view<TransformComponent, Rigidbody2D>())
		{
			auto& transform = m_components.get<TransformComponent>(entity);
			auto& rigidbody = m_components.get<Rigidbody2D>(entity);

			if (m_components.any_of<BoxCollider2D>(entity))
			{
				auto& collider = m_components.get<BoxCollider2D>(entity);

				if (collider.body == nullptr) collider.body = Physics2D::CreateBox(Entity{ entity, shared_from_this() }, collider);
				else
				{
					if (m_components.any_of<RelationshipComponent>(entity))
					{
						auto& relationship = m_components.get<RelationshipComponent>(entity);
						if (transform.changed)
							collider.body->SetPosition(transform.GetWorldPosition(relationship, m_components));
						else
							transform.SetWorldPosition(collider.body->GetPosition(), relationship, m_components);
						transform.SetWorldRotation(Quaternion::FromEuler(0.f, 0.f, collider.body->GetRotation()), relationship, m_components);
						transform.changed = false;
					}
					else
					{
						if (transform.changed)
							collider.body->SetPosition(transform.Position);
						else
							transform.SetLocalPosition(collider.body->GetPosition());
						transform.SetLocalRotation(Quaternion::FromEuler(0.f, 0.f, collider.body->GetRotation()));
						transform.changed = false;
					}
				}

				//Physics2D::SyncTransform(Entity(entity, this), transform, rigidbody, collider);
			}
			else if (m_components.any_of<CircleCollider2D>(entity))
			{
				auto& collider = m_components.get<CircleCollider2D>(entity);

				if (collider.body == nullptr) collider.body = Physics2D::CreateCircle(Entity{ entity, shared_from_this() }, collider);
				else
				{
					if (m_components.any_of<RelationshipComponent>(entity))
					{
						auto& relationship = m_components.get<RelationshipComponent>(entity);
						if (transform.changed)
							collider.body->SetPosition(transform.GetWorldPosition(relationship, m_components));
						else
							transform.SetWorldPosition(collider.body->GetPosition(), relationship, m_components);
						transform.SetWorldRotation(Quaternion::FromEuler(0.f, 0.f, collider.body->GetRotation()), relationship, m_components);
						transform.changed = false;
					}
					else
					{
						if (transform.changed)
							collider.body->SetPosition(transform.Position);
						else
							transform.SetLocalPosition(collider.body->GetPosition());
						transform.SetLocalRotation(Quaternion::FromEuler(0.f, 0.f, collider.body->GetRotation()));
						transform.changed = false;
					}
				}
				//Physics2D::SyncTransform(Entity(entity, this), transform, rigidbody, collider);
			}

		}
	}

	void Scene::UpdatePhysics()
	{
		//HL_PROFILE_BEGIN("Scene - Physics2D");
		/*HL_PROFILE_BEGIN("Scene - Physics2D - Submit");
		{
			auto view = m_components.view<TransformComponent, CircleCollider2D>(entt::exclude<DisabledObjectComponent>);

			for (auto entity : view)
			{
				auto [transform, collider] = view.get<TransformComponent, CircleCollider2D>(entity);

				if (m_components.any_of<Rigidbody2D>(entity))
				{
					auto& rigidbody = m_components.get<Rigidbody2D>(entity);

					Physics2D::Submit(Entity(entity, this), transform, rigidbody, collider);
				}
				else Physics2D::Submit(transform, collider);
			}
		}

		{
			auto view = m_components.view<TransformComponent, BoxCollider2D>(entt::exclude<DisabledObjectComponent>);
			for (auto entity : view)
			{
				auto [transform, collider] = view.get<TransformComponent, BoxCollider2D>(entity);
				if (m_components.any_of<Rigidbody2D>(entity))
				{
					auto& rigidbody = m_components.get<Rigidbody2D>(entity);
					Physics2D::Submit(Entity(entity, this), transform, rigidbody, collider);
				}
				else Physics2D::Submit(transform, collider);
			}
		}
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Scene - Physics2D - OnUpdate");
		Physics2D::DepricatedOnStep();
		HL_PROFILE_END();*/


		

		//HL_PROFILE_END();
	}

	void Scene::RenderScene(Ref<Framebuffer>& colorBuffer, TransformComponent world_camera, Matrix4x4 projection)
	{
		Transform::m_worldTransformCache.clear();
		auto directional_light_view = m_components.view<TransformComponent, DirectionalLightComponent>(entt::exclude<DisabledObjectComponent>);

		
		HL_PROFILE_BEGIN("Scene - Renderer2D");
		Renderer2D::BeginScene(projection);
		{
			auto view = m_components.view<TransformComponent, RelationshipComponent, SpriteRendererComponent>(entt::exclude<DisabledObjectComponent>);
			for (auto entity : view)
			{
				auto [transform, spriteRenderer] = view.get<TransformComponent, SpriteRendererComponent>(entity);

			retry:
				try {
					Renderer2D::DrawSprite((uint32_t)entity, Transform(entity, shared_from_this()).GetWorldTransformCache().GetModelMatrix(), spriteRenderer);
				}
				catch (HeliosException ex) {
					switch (Helios::ShowMessage("Error", ex.what(), Message::Flags::AbortRetryIgnore))
					{
					case Message::Result::Retry: goto retry;
					case Message::Result::Abort: Application::GetInstance().Quit(); break;
					}
				}
			}
		}
		Renderer2D::EndScene();
		HL_PROFILE_END();

		HL_PROFILE_BEGIN("Scene - Renderer3D");
		Renderer::BeginScene(colorBuffer, projection, world_camera.Position, {1.0f, 1.0f, 1.0f, 0.2f}, directional_light_view);
		HL_PROFILE_BEGIN("Scene - Renderer3D - Submit");
		{
			//auto view = m_components.view<TransformComponent, RelationshipComponent, MeshRendererComponent>(entt::exclude<DisabledObjectComponent>);
			//for (auto entity : view)
			//{
			//	auto [transform, meshRenderer, relationship] = view.get<TransformComponent, MeshRendererComponent, RelationshipComponent>(entity);

			//retry4:
			//	try {
			//		//Renderer::DrawMesh((uint32_t)entity, transform.GetWorldMatrixLight(relationship, m_components), meshRenderer);
			//		//Renderer::DrawMesh((uint64_t)entity, 0, Transform(entity, this).GetWorldTransformCache().GetModelMatrix(), meshRenderer);
			//	}
			//	catch (HeliosException ex) {
			//		switch (ex.what())
			//		{
			//		case IDRETRY: goto retry4;
			//		case IDABORT: Application::GetInstance().Quit(); break;
			//		}
			//	}
			//}
			

			m_components.group<TransformComponent, RelationshipComponent, MeshRendererComponent>(entt::exclude<DisabledObjectComponent>).each([&](auto entity, TransformComponent& transform, RelationshipComponent& relationship, MeshRendererComponent& meshRenderer)
			{
				TransformComponent transformComponent = Transform(entity, transform, relationship, shared_from_this()).GetWorldTransformCache();
				float dot = Vector3::Dot(world_camera.Forward(), transformComponent.Position - world_camera.Position);
				
				//if (dot < 0.0f) return;

				//static Matrix4x4 modelMatrix = Matrix4x4::Identity();
				//Renderer::SubmitMesh((uint64_t)entity, 0, modelMatrix, meshRenderer);
				Renderer::SubmitMesh((uint64_t)entity, 0, transformComponent.GetModelMatrix(), meshRenderer);
				//Renderer::DrawMesh((uint64_t)entity, 0, Transform(entity, transform, relationship, this).GetTransformComponent().GetModelMatrix(), meshRenderer);
				//Renderer::DrawMesh((uint64_t)entity, 0, Transform(entity, transform, relationship, this).GetWorldTransformCache().GetModelMatrix(), meshRenderer);
				//Renderer::DrawMesh((uint64_t)entity, 0, Transform(entity, this).GetWorldTransformCache().GetModelMatrix(), meshRenderer);
			});
		}
		HL_PROFILE_END();
		Renderer::EndScene();
		HL_PROFILE_END();
	}

#ifdef HELIOS_EDITOR
	void Scene::RenderGizmos(Matrix4x4 projection)
	{
		HL_PROFILE_BEGIN("Scene - Gizmos Renderer");
		GizmosRenderer::Begin(projection);
		//DepricatedApplication::instance->OnGizmosRender();
		GizmosRenderer::End();
		HL_PROFILE_END();
	}
#endif

	Entity& Scene::CreateMainCamera(Vector3 position) {
		Entity gameObject = CreateEntity("MainCamera", position);
		gameObject.AddScopedComponent<CameraComponent>().isPrimary = true;
		return gameObject;
	}

	Entity& Scene::CreateCamera(Vector3 position)
	{
		Entity gameObject = CreateEntity("MainCamera", position);
		gameObject.AddScopedComponent<CameraComponent>();
		return gameObject;
	}

	Vector2 DeserializeVector(std::string prefixX, std::string prefixY, YAML::Node in);
	void SerializeTransform(YAML::Emitter& out, Transform2DComponent& transform);

	void SerializeObject(YAML::Emitter& out, Entity& o)
	{
		out << YAML::BeginMap;

		//out << YAML::Key << "Name" << YAML::Value << o.m_name;

		SerializeTransform(out, o.GetComponent<Transform2DComponent>());

		out << YAML::EndMap;
	}

	void DeserializeObject(WeakRef<Scene> scene, YAML::Node& in)
	{
		if (in["Name"])
		{
			auto gm = scene.lock()->CreateEntity(in["Name"].as<std::string>().c_str());
			if (in["Transform"].IsMap())
			{
				auto transform = gm.GetComponent<Transform2DComponent>();
				transform.position = DeserializeVector("Position_X", "Position_Y", in["Transform"]);
				transform.size = DeserializeVector("Size_Width", "Size_Height", in["Transform"]);
			}
		}

	}

	void Scene::Shutdown()
	{
		m_components.clear();
	}

	void SerializeTransform(YAML::Emitter& out, Helios::Transform2DComponent& transform)
	{
		out << YAML::Key << "Transform" << YAML::BeginMap;

		out << YAML::Key << "Position_X" << YAML::Value << transform.position.x;
		out << YAML::Key << "Position_Y" << YAML::Value << transform.position.y;

		out << YAML::Key << "Size_Width" << YAML::Value << transform.size.x;
		out << YAML::Key << "Size_Height" << YAML::Value << transform.size.y;

		out << YAML::EndMap;
	}

	Vector2 DeserializeVector(std::string prefixX, std::string prefixY, YAML::Node in)
	{
		try {
			return { in[prefixX].as<float>(), in[prefixY].as<float>() };
		}
		catch (YAML::InvalidNode) {
			return { 0.0f, 0.0f };
		}
	}

	void Scene::Serialize(const std::string& filepath, WeakRef<Scene>& scene)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

		scene.lock()->m_components.each([&](entt::entity entity) {
			Entity gm = { entity, scene.lock() };
			SerializeObject(out, gm);
		});

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
	}

	void Helios::Scene::Deserialize(const std::string& filepath, Ref<Scene> scene)
	{
		auto in = YAML::LoadFile(filepath);

		if (!in.IsNull()) {
			if (in["GameObjects"].IsSequence()) {
				for (auto node : in["GameObjects"]) {
					DeserializeObject(scene, node);
				}
			}
		}
	}
}

