/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "Wallnut/Translation/Transform.h"
#include "ObjectComponent.h"
//#include "Wallnut/GameObjects/Components/Camera.h"
#include "Wallnut/Utils/LinkedList.h"
#include "Wallnut/Scene/SceneManager.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class ObjectComponent;
	class Camera;
	class WALLNUT_API GameObject
	{
	private:

		static std::vector<ObjectComponent*> components;
		static std::vector<GameObject*> gameObjects;
		static std::vector<GameObject*> globalGameObjects;

#pragma region Deletion Queue

		//static std::unordered_set<std::pair<GameObject*, int>> deletionQueueComponents;
		static std::unordered_set<int> deletionQueueGlobalGameObjects;
		static std::unordered_set<int> deletionQueueGameObjects;

#pragma endregion

		static void CheckQueue() {
			/*for (auto& x : deletionQueueComponents)
			{
				auto tmp = x.first->components[x.second];
				x.first->components.erase(x.first->components.begin() + x.second);
				delete tmp;
			}

			for (auto& i : deletionQueueGameObjects)
			{
				auto tmp = gameObjects[i];
				gameObjects.erase(gameObjects.begin() + i);
				delete tmp;
			}

			for (auto& i : deletionQueueGlobalGameObjects)
			{
				auto tmp = globalGameObjects[i];
				globalGameObjects.erase(globalGameObjects.begin() + i);
				delete tmp;
			}*/
		}

		static void prepare() {
			gameObjects.reserve(2000);
			globalGameObjects.reserve(100);
		}

		Transform transform;
		bool isGlobal = false;

		GameObject(): transform(this) {}
		
	public:

#pragma region Object Creation Helpers

		static GameObject& CreateMainCamera(Vector2D position = Vector2D(0.0f, 0.0f));
		static GameObject& CreateCamera(Vector2D position = Vector2D(0.0f, 0.0f));

#pragma endregion


		inline Transform& getTransform() {
			return transform;
		}

		template <class T = ObjectComponent>
		auto GetComponent() -> typename std::enable_if<std::is_base_of<ObjectComponent, T>::value, const T*>::type
		{
			static_assert(std::is_base_of<ObjectComponent, T>::value, "T must be a ObjectComponent");

			auto pos = std::find_if(components.begin(), components.end(), [](ObjectComponent* el) {
				return typeid(T) == typeid(*el);
			});

			/*if (pos != components.end())
				return dynamic_cast<T*>(reinterpret_cast<ObjectComponent*>((components[pos - components.begin()])));
			else */return nullptr;
		}

		void AddComponent(ObjectComponent& oc);

		void AddComponent(ObjectComponent* oc);

		static GameObject& InstantiateObject() {
			GameObject* obj = new GameObject();
			gameObjects.push_back(obj);
			return *obj;
		}

		// Serbia - Novi Sad has the worst healthcare personal that doesn't respect the laws and all and kill people by their actions

		static bool MakeGlobal(GameObject& gameObject) {
			auto position = std::find(gameObjects.begin(), gameObjects.end(), &gameObject);
			if (position != gameObjects.end())
			{
				gameObjects.erase(position);
				gameObject.isGlobal = true;
				globalGameObjects.push_back(&gameObject);
				return true;
			}
			else return false;
		}

		static bool Destroy(ObjectComponent* oc);

		static bool Destroy(GameObject& gameObject) { return DestroyObject(gameObject); }
		static bool DestroyGameObject(GameObject& gameObject) { return DestroyObject(gameObject); }
		static bool DestroyObject(GameObject& gameObject);

		friend class Application;
		friend class SceneManager;
	};
}

