/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Components.h"

namespace Helios {
	class Entity;
	class SceneRegistry;
	class Camera;
	class SceneCamera;
	class EditorCamera;
	class HELIOS_API Scene
	{
	private:
		std::string name;
		std::filesystem::path path;

		std::unordered_map<entt::entity, TransformComponent> m_worldTransformCache;
		
		entt::registry m_components;

		std::function<void(Scene&)> initCallback;

		std::unordered_set<entt::entity> m_entitiesToDestroy;

		std::unordered_map<UUID, entt::entity> m_UUIDMap;

	public:

		Scene() {};
		//Scene() = delete;
		Scene(std::string name) : name(name) { }
		Scene(std::string name, std::filesystem::path path) : name(name), path(path) { }
		~Scene();

		inline std::string GetName() const { return name; }

		/*template<typename... Component>
		static void CopyComponentIfExists(GameObject dst, GameObject src)
		{
			([&]()
				{
					if (src.HasComponent<Component>())
						dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
				}(), ...);
		}*/

		inline bool contains(entt::entity entity);

		Entity GetEntity(UUID uuid);

		Entity GetEntity(entt::entity entity);

		Entity DuplicateEntity(Entity entity);
		Entity CopyEntity(Entity entity);

		uint64_t GetEntityCount() const { return m_components.size(); }

		void PerformCleanupAndSync();
		void UpdatePhysics();

		inline void RenderScene(SceneCamera camera);
		void RenderScene(EditorCamera& camera);
		void RenderScene(TransformComponent world_camera, Matrix4x4 projection);
#ifdef HELIOS_EDITOR
		void RenderGizmos(Matrix4x4 projection);
#endif
		Entity InstantiateObject();
		Entity InstantiateObject(Vector3 position);
		Entity InstantiateObject(entt::entity parent);
		Entity InstantiateObject(std::string name, Vector3 position = Vector3::Zero());
		Entity InstantiateObject(std::string name, entt::entity& parent);
		Entity& CreateMainCamera(Vector3 position);
		Entity& CreateCamera(Vector3 position);

		void Init() { if(initCallback) initCallback(*this); }
		void Shutdown();
		
		template <typename... T>
		decltype(auto) GetComponents()
		{
			return m_components.template view<T...>();
		}


		static void UpdateChildTransforms(Ref<Scene> scene);

		static void Serialize(const std::string& filepath, WeakRef<Scene>& scene);
		static void Deserialize(const std::string& filepath, Ref<Scene> scene);

		/*static void SerializeBinary(std::filesystem::path path, Scene& scene);
		static void DeserializeBinary(std::filesystem::path path, Scene& scene);*/

		friend class SceneManager;
		friend class Entity;
		friend class Transform;
		friend class SceneSerializer;

		friend void DeserializeEntity(YAML::Node entityNode, Scene* scene);
	};
}