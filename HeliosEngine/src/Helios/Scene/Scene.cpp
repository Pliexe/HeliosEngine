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

	Scene::Scene() = default;
	Scene::~Scene() {
		   
	}
	
	entt::registry Scene::s_components;
	
	inline Helios::Components::Camera& Scene::GetPrimaryCamera()
	{
		HL_CORE_ASSERT_WITH_MSG(primaryCamera != entt::null, "Primary Camera is entt::null while trying to get it!!!");
		return m_components.get<Components::Camera>(primaryCamera);
	}

	inline  void Scene::SetPrimaryCamera(GameObject& obj)
	{
		HL_CORE_ASSERT_WITH_MSG(obj.HasComponent<Components::Camera>(), "The Object has no Camera component!");
		primaryCamera = obj;
	}

	inline void Scene::ResetPrimaryCamera() { primaryCamera = entt::null; }

	void Scene::OnUpdateRuntime()
	{
		if (primaryCamera != entt::null) {
			auto& relationship = GameObject(primaryCamera).GetComponent<Components::Relationship>();

			Matrix4x4 projection;

			if (relationship.parent_handle != entt::null)
			{
				auto parent = GameObject(relationship.parent_handle);

				projection = SceneCamera::GetProjection(
					GameObject(primaryCamera).GetComponent<Components::Transform>(),
					GameObject(primaryCamera).GetComponent<Components::Camera>()
				);
			} else projection = SceneCamera::GetProjection(
				GameObject(primaryCamera).GetComponent<Components::Transform>(),
				GameObject(primaryCamera).GetComponent<Components::Camera>()
			);

			Renderer2D::BeginScene(projection);

			{
				//Renderer2D::DrawPolygon((sin(Time::passedTime()) + 1) * 50);

				auto view = m_components.view<Components::Transform, Components::Relationship, Components::SpriteRenderer>();
				for (auto entity : view)
				{
					auto [trans, relt, spriteRenderer] = view.get<Components::Transform, Components::Relationship, Components::SpriteRenderer>(entity);

				retry:
					try {
						if (relt.parent_handle != entt::null)
						{
							Components::Transform tmp = trans;
							tmp.position += GameObject(relt.parent_handle).GetComponent<Components::Transform>().position;
							// Renderer2D::DrawSprite(tmp, spriteRenderer);
							Renderer2D::DrawSprite((uint32_t)entity, trans, spriteRenderer);
						}
						else
							Renderer2D::DrawSprite((uint32_t)entity, trans, spriteRenderer);
							// Renderer2D::DrawSprite(trans, spriteRenderer);
					}
					catch (HeliosExceptin ex) {
						switch (ex.what())
						{
						case IDRETRY: goto retry;
						case IDABORT: Application::Quit(); break;
						}
					}

					//Renderer2D::DrawTriangle(trans.position);
				}

				//Renderer2D::DrawTriangle(Vector2());

				//Graphics::instance->m_renderTarget2D->Clear(camera.clear_color);

				//auto group = m_components.group <Components::Transform, Components::
			}

			Renderer2D::EndScene();

			Renderer::BeginScene(projection);

			{
				auto view = m_components.view<Components::Transform, Components::Relationship, Components::MeshRenderer>();
				for (auto entity : view)
				{
					auto [trans, relt, meshRenderer] = view.get<Components::Transform, Components::Relationship, Components::MeshRenderer>(entity);

				retry4:
					try {
						if (relt.parent_handle != entt::null)
						{
							Components::Transform tmp = trans;
							tmp.position += GameObject(relt.parent_handle).GetComponent<Components::Transform>().position;
							Renderer::DrawMesh((uint32_t)entity, tmp, meshRenderer);
						}
						else
							Renderer::DrawMesh((uint32_t)entity, trans, meshRenderer);
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
	}

	void Scene::OnUpdateEditor(SceneCamera camera)
	{
		if (primaryCamera != entt::null) {

			auto projection = camera.GetProjection();

			Renderer2D::BeginScene(projection);

			{
				//Renderer2D::DrawPolygon((sin(Time::passedTime()) + 1) * 50);

				auto view = m_components.view<Components::Transform, Components::Relationship, Components::SpriteRenderer>(entt::exclude<Components::DisabledObject>);
				for (auto entity : view)
				{
					auto [trans, relt, spriteRenderer] = view.get<Components::Transform, Components::Relationship, Components::SpriteRenderer>(entity);

				retry2d:
					try {
						if (relt.parent_handle != entt::null)
						{
							Components::Transform tmp = trans;
							tmp.position += GameObject(relt.parent_handle).GetComponent<Components::Transform>().position;
							Renderer2D::DrawSprite((uint32_t)entity, tmp, spriteRenderer);
						}
						else
							Renderer2D::DrawSprite((uint32_t)entity, trans, spriteRenderer);
					}
					catch (HeliosExceptin ex) {
						switch (ex.what())
						{
						case IDRETRY: goto retry2d;
						case IDABORT: Application::Quit(); break;
						}
					}

					//Renderer2D::DrawTriangle(trans.position);
				}

				//Renderer2D::DrawTriangle(Vector2());

				//Graphics::instance->m_renderTarget2D->Clear(camera.clear_color);

				//auto group = m_components.group <Components::Transform, Components::
			}

			Renderer2D::EndScene();


			Renderer::BeginScene(projection);

			{
				auto view = m_components.view<Components::Transform, Components::Relationship, Components::MeshRenderer>(entt::exclude<Components::DisabledObject>);
				for (auto entity : view)
				{
					auto [trans, relt, meshRenderer] = view.get<Components::Transform, Components::Relationship, Components::MeshRenderer>(entity);

					retry:
					try {
						if (relt.parent_handle != entt::null)
						{
							Components::Transform tmp = trans;
							tmp.position += GameObject(relt.parent_handle).GetComponent<Components::Transform>().position;
							Renderer::DrawMesh((uint32_t)entity, tmp, meshRenderer);
						}
						else
							Renderer::DrawMesh((uint32_t)entity, trans, meshRenderer);
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

			GizmosRenderer::Begin(projection);
			Application::instance->OnGizmosRender();
			GizmosRenderer::End();
		}
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

	void DeserializeObject(YAML::Node& in)
	{

		if (in["Name"])
		{
			GameObject gm = GameObject::InstantiateObject(in["Name"].as<std::string>().c_str());
			if (in["Transform"].IsMap())
			{
				auto transform = gm.GetComponent<Components::Transform2D>();
				transform.position = DeserializeVector("Position_X", "Position_Y", in["Transform"]);
				transform.size = DeserializeVector("Size_Width", "Size_Height", in["Transform"]);
			}
		}

	}

	void Helios::Scene::Unload()
	{

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
			GameObject gm = entity;
			SerializeObject(out, gm);
		});

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
	}

	void Helios::Scene::Deserialize(const std::string& filepath)
	{
		auto in = YAML::LoadFile(filepath);

		if (!in.IsNull()) {
			if (in["GameObjects"].IsSequence()) {
				for (auto node : in["GameObjects"]) {
					DeserializeObject(node);
				}
			}
		}
	}
}

