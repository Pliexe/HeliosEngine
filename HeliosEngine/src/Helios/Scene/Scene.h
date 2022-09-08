/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios {
	class GameObject;
	class SceneManager;
	class Camera;
	class HELIOS_API Scene
	{
	private:


		entt::registry m_components;
		static entt::registry s_components;

		Camera* currentCamera = NULL;
		std::function<void(Scene&)> initCallback;

		//LinkedList<std::shared_ptr<GameObject>> gameObjects;

	public:

		Scene();
		~Scene();

		void RenderScene();

		void Init() { initCallback(*this); }

		void Unload();

		static void Serialize(const std::string& filepath, WeakRef<Scene>& scene);
		static void Deserialize(const std::string& filepath);

		/*static void SerializeBinary(std::filesystem::path path, Scene& scene);
		static void DeserializeBinary(std::filesystem::path path, Scene& scene);*/

		friend class SceneManager;
		friend class GameObject;
		friend class Transform;

		extern friend class GameEngine;
		extern friend class HierarchyPanel;
	};


}