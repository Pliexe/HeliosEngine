/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Helios/Translation/Transform.h"
#include "Helios/Translation/Vector2D.h"
#include "Helios/Scene/GameObject.h"
#include "Helios/Renderer/Renderer2D.h"
#include "Scene.h"
#include "Helios/Core/Time.h"

#include "Components.h"

namespace Helios {

	Scene::Scene() = default;
	Scene::~Scene() {
		   
	}
	
	entt::registry Scene::s_components;

	void Scene::RenderScene()
	{
		Components::Camera camera;
		Components::Transform2D transform;

		
		auto view = m_components.view<Components::Transform2D, Components::Camera>();
		for (auto entity : view)
		{
			auto [t_transform, t_camera] = view.get<Components::Transform2D, Components::Camera>(entity);

			camera = t_camera;
			transform = t_transform;
		}

		Renderer2D::BeginScene(camera);

		{

			auto view = m_components.view<Components::Transform2D, Components::Relationship, Components::SpriteRenderer>();
			for (auto entity : view)
			{
				auto [trans, relt, spriteRenderer] = view.get<Components::Transform2D, Components::Relationship, Components::SpriteRenderer>(entity);
				
				//Renderer2D::DrawTriangle(trans.position);
			}

			//Renderer2D::DrawTriangle(Vector2D());
			Renderer2D::DrawHexagon((sin(Time::passedTime()) + 1) * 50);

			//Graphics::instance->m_renderTarget2D->Clear(camera.clear_color);

			//auto group = m_components.group <Components::Transform, Components::
		}

		Renderer2D::EndScene();
	}

	Vector2D DeserializeVector(std::string prefixX, std::string prefixY, YAML::Node in);
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

		out << YAML::Key << "Position_X" << YAML::Value << transform.position.getX();
		out << YAML::Key << "Position_Y" << YAML::Value << transform.position.getY();

		out << YAML::Key << "Size_Width" << YAML::Value << transform.size.getX();
		out << YAML::Key << "Size_Height" << YAML::Value << transform.size.getY();

		out << YAML::EndMap;
	}

	Vector2D DeserializeVector(std::string prefixX, std::string prefixY, YAML::Node in)
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

