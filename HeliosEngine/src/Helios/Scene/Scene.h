/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Components.h"
#include "EntityContainer.h"

namespace Helios {
	class Entity;
	class SceneRegistry;
	class Camera;
	class SceneCamera;
	class EditorCamera;
	class Framebuffer;
	class HELIOS_API Scene : public EntityContainer
	{
	private:
		std::string name;
		std::filesystem::path path;

		std::function<void(Scene&)> initCallback;

		std::unordered_set<entt::entity> m_entitiesToDestroy;

	public:

		Scene() {};
		//Scene() = delete;
		Scene(std::string name) : name(name) { }
		Scene(std::string name, std::filesystem::path path) : name(name), path(path) { }
		~Scene();

		inline std::string GetName() const { return name; }

		void PerformCleanupAndSync();
		void UpdatePhysics();

		void RenderScene(Ref<Framebuffer>& colorBuffer, EditorCamera& camera);
		void RenderScene(Ref<Framebuffer>& colorBuffer, TransformComponent world_camera, Matrix4x4 projection);
#ifdef HELIOS_EDITOR
		void RenderGizmos(Matrix4x4 projection);
#endif
		Entity& CreateMainCamera(Vector3 position);
		Entity& CreateCamera(Vector3 position);

		void Init() { if(initCallback) initCallback(*this); }
		void Shutdown();

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