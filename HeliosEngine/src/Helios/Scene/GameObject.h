/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Scene/SceneRegistry.h"
#include "Helios/Scene/Scene.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Asserts.h"

#include "Components.h"

namespace Helios {
	class Camera;
	class Graphics;
	class Scene; 
	class HELIOS_API GameObject
	{
	public:

		//GameObject() = default;
		GameObject() { }
		GameObject(const GameObject& other) = default;
		GameObject(entt::entity entity_id, Ref<Scene> scene)
		{
			HL_CORE_ASSERT_WITH_MSG(entity_id != entt::null, "entity handle is invalid!");
			m_scene = scene.get();
			m_entityHandle = entity_id;
		}
		GameObject(entt::entity entity_id, Scene* scene)
		{
			HL_CORE_ASSERT_WITH_MSG(entity_id != entt::null, "entity handle is invalid!");
			m_scene = scene;
			m_entityHandle = entity_id;
		}

		template <typename... T>
		bool HasComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(m_entityHandle != entt::null, "entity handle is invalid!")
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!")
			return m_scene->m_components.template any_of<T...>(m_entityHandle);
		}

		template <typename T, typename... Args>
		T& AddComponent(Args &&...args)
		{
			HL_CORE_ASSERT_WITH_MSG(!HasComponent<T>(), "GameObject does not have the component!");
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!")
			T& comp = m_scene->m_components.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
			return comp;
		}

		template <typename T>
		void RemoveComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(HasComponent<T>(), "GameObject does not have the component!");
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!");
			m_scene->m_components.remove<T>(m_entityHandle);
		}

		template <typename... T>
		decltype(auto) GetComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(HasComponent<T...>(), "GameObject does not have the component!");
			//HL_CORE_ASSERT_WITH_MSG(!m_scene.expired(), "Scene does not exist anymore!");
			return m_scene->m_components.template get<T...>(m_entityHandle);
		}

		operator entt::entity() const { return m_entityHandle; }
		bool operator==(const GameObject& other) const { return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene; }
		bool operator==(const entt::entity& other) const { return m_entityHandle == other; }
		bool operator==(const Scene* other) const { return m_scene == other; }
		bool operator==(const Ref<Scene> other) const { return m_scene == other.get(); }
		bool operator==(const WeakRef<Scene> other) const { return m_scene == other.lock().get(); }

		inline bool isGlobal() { return HasComponent<Components::GlobalObject>(); }
		void MakeGlobal();
		void Destroy();
		inline bool IsValid() { return m_entityHandle != entt::null && m_scene->m_components.valid(m_entityHandle); }
		inline bool IsNotNull() { return m_entityHandle != entt::null; }
		inline bool IsNull() { return m_entityHandle == entt::null; }

	private:

		Scene* m_scene;
		entt::entity m_entityHandle{ entt::null };
		
	public:

#pragma region GameObject Creation Helpers

		GameObject& CreateMainCamera(Vector2 position = Vector2(0.0f, 0.0f));
		GameObject& CreateCamera(Vector2 position = Vector2(0.0f, 0.0f));

#pragma endregion

		void ResetParent();
		void SetParent(GameObject& object);
		void SetParent(entt::entity& object);

		inline std::string& GetName() {
			return GetComponent<Components::InfoComponent>().name;
		}

		inline Components::Transform2D GetTransform() {
			return GetComponent<Components::Transform2D>();
		}

		GameObject InstantiateObject();
		GameObject InstantiateObject(std::string name);
		GameObject InstantiateObject(GameObject& parent);
		GameObject InstantiateObject(std::string name, GameObject& parent);
		
		// Serbia - Novi Sad has the worst healthcare personal that doesn't respect the laws and all and kill people by their actions
		
		friend struct Components::Transform2D;

		friend class Application;
		friend class SceneManager;
		friend class Scene;
		friend void SerializeObject(YAML::Emitter& out, GameObject& o);

		extern friend class GameEngine;
		extern friend class InspectorPanel;
	};
}

