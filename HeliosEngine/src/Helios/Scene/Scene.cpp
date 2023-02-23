/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Helios/Translation/Vector.h"
#include "Helios/Core/Application.h"
#include "Helios/Scene/GameObject.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Renderer.h"
#include "Helios/Graphics/Renderer2D.h"
#include "Scene.h"
#include "Helios/Core/Time.h"

#include "Components.h"
#include "Helios/Graphics/GizmosRenderer.h"

namespace Helios {

	Scene::~Scene() {
		Application::ShowMessage("Test","Scene Deleted!");
	}

	bool Scene::contains(entt::entity entity)
	{
		return m_components.valid(entity);
	}

	void Scene::UpdateChildTransforms(Ref<Scene> scene)
	{
		auto view = scene->m_components.view<Components::Transform, Components::Relationship>();
		for (auto entity : view)
		{
			auto [transform, relationship] = view.get<Components::Transform, Components::Relationship>(entity);

			if (relationship.HasParent()) continue;
			
			static std::function<void(entt::entity, Components::Relationship&, Components::Transform&, Components::Transform&)> UpdateTransform = [scene](entt::entity entity, Components::Relationship& relationship, Components::Transform& transform, Components::Transform& parentTransform)
			{
				transform.position = transform.localPosition + parentTransform.localPosition;
				transform.rotation = transform.localRotation * parentTransform.localRotation;
				transform.scale = {
					transform.localScale.x * parentTransform.localScale.x,
					transform.localScale.y * parentTransform.localScale.y,
					transform.localScale.z * parentTransform.localScale.z
				};

				if (relationship.first_child != entt::null)
				{
					auto [childTransform, childRelationship] = scene->m_components.get<Components::Transform, Components::Relationship>(relationship.first_child);
					UpdateTransform(relationship.first_child, childRelationship, childTransform, transform);
				}
				if(relationship.next_child != entt::null)
				{
					auto [childTransform, childRelationship] = scene->m_components.get<Components::Transform, Components::Relationship>(relationship.next_child);
					UpdateTransform(relationship.next_child, childRelationship, childTransform, transform);
				}
				if(relationship.prev_child != entt::null)
				{
					auto [childTransform, childRelationship] = scene->m_components.get<Components::Transform, Components::Relationship>(relationship.prev_child);
					UpdateTransform(relationship.prev_child, childRelationship, childTransform, transform);
				}
			};

			if (relationship.first_child != entt::null)
			{
				auto [childTransform, childRelationship] = scene->m_components.get<Components::Transform, Components::Relationship>(relationship.first_child);
				UpdateTransform(relationship.first_child, childRelationship, childTransform, transform);
			}
		}
	}

	void Scene::RenderScene(SceneCamera camera)
	{
		Matrix4x4 projection = camera.GetProjection();
		RenderScene(projection);
		//RenderGizmos(projection);
	}
	void Scene::RenderScene(Matrix4x4 projection)
	{
		auto directional_light_view = m_components.view<Components::Transform, Components::DirectionalLight>(entt::exclude<Components::DisabledObject>);

		Renderer2D::BeginScene(projection);
		{
			auto view = m_components.view<Components::Transform, Components::Relationship, Components::SpriteRenderer>(entt::exclude<Components::DisabledObject>);
			for (auto entity : view)
			{
				auto [transform, spriteRenderer] = view.get<Components::Transform, Components::SpriteRenderer>(entity);

			retry:
				try {
					Renderer2D::DrawSprite((uint32_t)entity, transform, spriteRenderer);
				}
				catch (HeliosExceptin ex) {
					switch (ex.what())
					{
					case IDRETRY: goto retry;
					case IDABORT: Application::Quit(); break;
					}
				}
			}
		}
		Renderer2D::EndScene();

		Renderer::BeginScene(projection, { 1.0f, 1.0f, 1.0f, 1.0f }, directional_light_view);

		{
			auto view = m_components.view<Components::Transform, Components::Relationship, Components::MeshRenderer>(entt::exclude<Components::DisabledObject>);
			for (auto entity : view)
			{
				auto [transform, meshRenderer] = view.get<Components::Transform, Components::MeshRenderer>(entity);

			retry4:
				try {
					Renderer::DrawMesh((uint32_t)entity, transform, meshRenderer);
				}
				catch (HeliosExceptin ex) {
					switch (ex.what())
					{
					case IDRETRY: goto retry4;
					case IDABORT: Application::Quit(); break;
					}
				}
			}
		}
	}

#ifdef HELIOS_EDITOR
	void Scene::RenderGizmos(Matrix4x4 projection)
	{
		GizmosRenderer::Begin(projection);
		Application::instance->OnGizmosRender();
		GizmosRenderer::End();
	}
#endif

	GameObject Scene::InstantiateObject()
	{
		return InstantiateObject("GameObject (" + std::to_string(m_components.size() + 1) + ")");
	}

	GameObject Scene::InstantiateObject(Vector3 position)
	{
		return InstantiateObject("GameObject (" + std::to_string(m_components.size() + 1) + ")", position);
	}

	GameObject Scene::InstantiateObject(entt::entity& parent)
	{
		return InstantiateObject("GameObject (" + std::to_string(m_components.size() + 1) + ")", parent);
	}

	GameObject Scene::InstantiateObject(std::string name, Vector3 position)
	{
		GameObject obj(m_components.create(), this);
		obj.AddComponent<Components::InfoComponent>(name);
		obj.AddComponent<Components::Transform>(position);
		obj.AddComponent<Components::Relationship>();
		return obj;
	}

	GameObject Scene::InstantiateObject(std::string name, entt::entity& parent)
	{
		GameObject obj(m_components.create(), this);
		obj.AddComponent<Components::InfoComponent>(name);
		obj.AddComponent<Components::Transform>();
		obj.AddComponent<Components::Relationship>(m_components, obj, parent);
		return obj;
	}

	GameObject& Scene::CreateMainCamera(Vector3 position) {
		GameObject gameObject = InstantiateObject("MainCamera", position);
		gameObject.AddComponent<Components::Camera>().isPrimary = true;
		return gameObject;
	}

	GameObject& Scene::CreateCamera(Vector3 position)
	{
		GameObject gameObject = InstantiateObject("MainCamera", position);
		gameObject.AddComponent<Components::Camera>();
		return gameObject;
	}

	Vector2 DeserializeVector(std::string prefixX, std::string prefixY, YAML::Node in);
	void SerializeTransform(YAML::Emitter& out, Components::Transform2D& transform);

	void SerializeObject(YAML::Emitter& out, GameObject& o)
	{
		out << YAML::BeginMap;

		//out << YAML::Key << "Name" << YAML::Value << o.m_name;

		SerializeTransform(out, o.GetComponent<Components::Transform2D>());

		out << YAML::EndMap;
	}

	void DeserializeObject(WeakRef<Scene> scene, YAML::Node& in)
	{
		if (in["Name"])
		{
			auto gm = scene.lock()->InstantiateObject(in["Name"].as<std::string>().c_str());
			if (in["Transform"].IsMap())
			{
				auto transform = gm.GetComponent<Components::Transform2D>();
				transform.position = DeserializeVector("Position_X", "Position_Y", in["Transform"]);
				transform.size = DeserializeVector("Size_Width", "Size_Height", in["Transform"]);
			}
		}

	}

	void Scene::Shutdown()
	{
		m_components.clear();
	}

	void SerializeTransform(YAML::Emitter& out, Helios::Components::Transform2D& transform)
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
			GameObject gm = { entity, scene.lock() };
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

