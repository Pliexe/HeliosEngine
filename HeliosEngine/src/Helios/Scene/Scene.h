/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Components.h"
#include "SceneCamera.h"

namespace Helios {
	class GameObject;
	class SceneRegistry;
	class Camera;

	class HELIOS_API Scene
	{
	private:
		std::string name;
		std::filesystem::path path;

		entt::registry m_components;

		std::function<void(Scene&)> initCallback;

	public:

		Scene() {};
		//Scene() = delete;
		Scene(std::string name) : name(name) { }
		Scene(std::string name, std::filesystem::path path) : name(name), path(path) { }
		~Scene();

		inline bool contains(entt::entity entity);
		
		inline void RenderScene(SceneCamera camera);
		void RenderScene(Matrix4x4 projection);
#ifdef HELIOS_EDITOR
		void RenderGizmos(Matrix4x4 projection);
#endif
		GameObject InstantiateObject();
		GameObject InstantiateObject(Vector3 position);
		GameObject InstantiateObject(entt::entity& parent);
		GameObject InstantiateObject(std::string name, Vector3 position = Vector3::Zero());
		GameObject InstantiateObject(std::string name, entt::entity& parent);
		GameObject& CreateMainCamera(Vector3 position);
		GameObject& CreateCamera(Vector3 position);

		void Init() { if(initCallback) initCallback(*this); }
		void Shutdown();
		
		template <typename... T>
		decltype(auto) GetComponents()
		{
			return m_components.view<T...>();
		}

		static void UpdateChildTransforms(Ref<Scene> scene);

		static void Serialize(const std::string& filepath, WeakRef<Scene>& scene);
		static void Deserialize(const std::string& filepath, Ref<Scene> scene);

		/*static void SerializeBinary(std::filesystem::path path, Scene& scene);
		static void DeserializeBinary(std::filesystem::path path, Scene& scene);*/

		friend class SceneManager;
		friend class GameObject;
		friend class Transform;

		extern friend class GameEngine;
		extern friend class HierarchyPanel;
	};
}