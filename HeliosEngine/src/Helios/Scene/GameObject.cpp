/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "GameObject.h"
#include "Helios/GameObjects/ObjectBehaviour.h"

#include "entt.hpp"

namespace Helios 
{	

	GameObject GameObject::InstantiateObject()
	{
		const WeakRef<Scene>& scene = SceneManager::GetCurrentScene();
		HL_CORE_ASSERT_WITH_MSG(!scene.expired(), "Can't instatiate object without a scene!");
		return InstantiateObject("GameObject (" + std::to_string(scene.lock()->m_components.size() + 1) + ")");
	}

	GameObject GameObject::InstantiateObject(GameObject& parent)
	{
		const WeakRef<Scene>& scene = SceneManager::GetCurrentScene();
		HL_CORE_ASSERT_WITH_MSG(!scene.expired(), "Can't instatiate object without a scene!");
		return InstantiateObject("GameObject (" + std::to_string(scene.lock()->m_components.size() + 1) + ")", parent);
	}

	GameObject GameObject::InstantiateObject(std::string name)
	{
		WeakRef<Scene> currentScene = SceneManager::GetCurrentScene();
		HL_CORE_ASSERT_WITH_MSG(!currentScene.expired(), "Can't instantiate object without a scene being loaded!");
		GameObject obj = currentScene.lock()->m_components.create();
		obj.AddComponent<Components::InfoComponent>(name);
		obj.AddComponent<Components::Transform2D>();
		obj.AddComponent<Components::Relationship>();
		return obj;
	}

	GameObject GameObject::InstantiateObject(std::string name, GameObject& parent)
	{
		WeakRef<Scene> currentScene = SceneManager::GetCurrentScene();
		HL_CORE_ASSERT_WITH_MSG(!currentScene.expired(), "Can't instantiate object without a scene being loaded!");
		GameObject obj = currentScene.lock()->m_components.create();
		obj.AddComponent<Components::InfoComponent>(name);
		obj.AddComponent<Components::Relationship>(currentScene.lock()->m_components, obj, parent);
		return obj;
	}

	void GameObject::SetParent(GameObject& object)
	{
		if (m_global)
			GetComponent<Components::Relationship>().SetParent(Scene::s_components, m_entityHandle, object);
		else {
			WeakRef<Scene> scene = SceneManager::GetCurrentScene();
			HL_CORE_ASSERT_WITH_MSG(!scene.expired(), "Cannot reset Parent while Scene is unloaded!");
			GetComponent<Components::Relationship>().SetParent(scene.lock()->m_components, m_entityHandle, object);
		}
	}

	void GameObject::ResetParent()
	{
		if (m_global)
			GetComponent<Components::Relationship>().Reset(Scene::s_components);
		else {
			WeakRef<Scene> scene = SceneManager::GetCurrentScene();
			HL_CORE_ASSERT_WITH_MSG(!scene.expired(), "Cannot reset Parent while Scene is unloaded!");
			GetComponent<Components::Relationship>().Reset(scene.lock()->m_components);
		}
	}

	//Transform2D GameObject::relationship = Transform2D();

	GameObject& GameObject::CreateMainCamera(Vector2D position) {
		GameObject gameObject = InstantiateObject("MainCamera");
		gameObject.AddComponent<Components::Camera>();
		// TODO
		// SET MAIN CAMERA 
		return gameObject;
	}

	GameObject& GameObject::CreateCamera(Vector2D position)
	{
		GameObject gameObject = InstantiateObject("MainCamera");
		gameObject.AddComponent<Components::Camera>();
		return gameObject;
	}

	void GameObject::DestroyObject(GameObject gameObject)
	{
		if (gameObject.m_global)
		{
			if (gameObject.HasComponent<Components::Relationship>())
				gameObject.GetComponent<Components::Relationship>().Reset(Scene::s_components);
			Scene::s_components.destroy(gameObject);
		}
		else {
			const WeakRef<Scene>& scene = SceneManager::GetCurrentScene();
			HL_CORE_ASSERT_WITH_MSG(!scene.expired(), "Can't destory GameObject if a scene is unloaded!");

			if (gameObject.HasComponent<Components::Relationship>())
				gameObject.GetComponent<Components::Relationship>().Reset(scene.lock()->m_components);

			scene.lock()->m_components.destroy(gameObject);
		}
	}

	bool GameObject::MakeGlobal(GameObject& gameObject)
	{
		if (gameObject.m_global) return true;
		
		gameObject.m_entityHandle = Scene::s_components.create();
		gameObject.m_global = true;
		return true;
	}
}
