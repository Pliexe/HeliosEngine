/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Helios/Scene/SceneManager.h"
#include "Helios/Scene/Scene.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Asserts.h"

#include "Components.h"

namespace Helios {
	class Camera;
	class Graphics;
	class HELIOS_API GameObject
	{

	public:

		GameObject() = delete;
		GameObject(entt::entity entity_id)
		{
			HL_CORE_ASSERT_WITH_MSG(entity_id != entt::null, "entity handle is invalid!");
			m_entityHandle = entity_id;
		}

		template <typename T>
		bool HasComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(m_entityHandle != entt::null, "entity handle is invalid!")
			if (m_global) {
				return Scene::s_components.any_of<T>(m_entityHandle);
			}
			else if (auto tmp = SceneManager::GetCurrentScene().lock())
			{
				return tmp->m_components.any_of<T>(m_entityHandle);
			}
			else return false;
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HL_CORE_ASSERT_WITH_MSG(!HasComponent<T>(), "GameObject does not have the component!");
			return (m_global ? Scene::s_components : SceneManager::GetCurrentScene().lock()->m_components).emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		T& GetComponent()
		{
			HL_CORE_ASSERT_WITH_MSG(HasComponent<T>(), "GameObject does not have the component!");
			return (m_global ? Scene::s_components : SceneManager::GetCurrentScene().lock()->m_components).get<T>(m_entityHandle);
		}

		operator entt::entity() const { return m_entityHandle; }

	private:

		bool m_global = false;
		entt::entity m_entityHandle;
		
	public:

#pragma region GameObject Creation Helpers

		static GameObject& CreateMainCamera(Vector2 position = Vector2(0.0f, 0.0f));
		static GameObject& CreateCamera(Vector2 position = Vector2(0.0f, 0.0f));

#pragma endregion

		void ResetParent();
		void SetParent(GameObject& object);

		inline std::string& GetName() {
			return GetComponent<Components::InfoComponent>().name;
		}

		inline Components::Transform2D GetTransform() {
			return GetComponent<Components::Transform2D>();
		}

		static GameObject InstantiateObject();
		static GameObject InstantiateObject(std::string name);
		static GameObject InstantiateObject(GameObject& parent);
		static GameObject InstantiateObject(std::string name, GameObject& parent);
		
		// Serbia - Novi Sad has the worst healthcare personal that doesn't respect the laws and all and kill people by their actions

		static bool MakeGlobal(GameObject& gameObject);

		static void DestroyObject(GameObject gameObject);

		friend struct Components::Transform2D;

		friend class Application;
		friend class SceneManager;
		friend class Scene;
		friend void SerializeObject(YAML::Emitter& out, GameObject& o);

		extern friend class GameEngine;
		extern friend class InspectorPanel;
	};
}

