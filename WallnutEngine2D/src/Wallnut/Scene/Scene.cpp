/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Scene.h"
#include "Wallnut/Translation/Transform.h"
#include "Wallnut/Translation/Vector2D.h"

void Wallnut::Scene::Unload()
{

}

void SerializeTransform(YAML::Emitter& out, Wallnut::Transform& transform)
{
	out << YAML::BeginMap;

	const Wallnut::Vector2D& pos = transform.getPosition();
	const Wallnut::Vector2D& size= transform.getSize();

	out << YAML::Key << "Position_X" << YAML::Value << pos.getX();
	out << YAML::Key << "Position_Y" << YAML::Value << pos.getY();

	out << YAML::Key << "Size_Width" << YAML::Value << size.getX();
	out << YAML::Key << "Size_Height" << YAML::Value << size.getY();

	out << YAML::EndMap;
}

void Wallnut::SerializeObject(YAML::Emitter& out, GameObject& o)
{
	out << YAML::BeginMap;

	out << YAML::Key << "Name" << YAML::Value << o.m_name;

	SerializeTransform(out, o.getTransform());

	out << YAML::EndMap;
}

void Wallnut::Scene::Serialize(const std::string& filepath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Version" << YAML::Value << SCENE_FILE_VERSION;
	out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

	for (auto& o : GameObject::gameObjects)
	{
		SerializeObject(out, *o);
	}

	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
	fout.close();
}

void Wallnut::Scene::Deserialize(const std::string& filepath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Version" << YAML::Value << SCENE_FILE_VERSION;
	out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

	for (auto& o : GameObject::gameObjects)
	{
		SerializeObject(out, *o);
	}

	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
	fout.close();
}