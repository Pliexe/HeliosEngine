/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "pch.h"
#include "ObjectComponent.h"
#include "Wallnut/Utils/LinkedList.h"
#include "Wallnut/Scene/SceneManager.h"
#include "Wallnut/Core.h"

namespace Wallnut {
	class WALLNUT_API GameObject
	{
	private:

		LinkedList<ObjectComponent> components;

		static std::vector<GameObject*> gameObjects;
		static std::vector<GameObject*> globalGameObjects;
		static void prepare() {
			gameObjects.reserve(2000);
			globalGameObjects.reserve(100);
		}

		GameObject() {}
		
	public:

		static GameObject& InstantiateObject() {
			GameObject* obj = new GameObject();
			gameObjects.push_back(obj);
			return *obj;
		}

		static bool MakeGlobal(GameObject& gameObject) {
			auto position = std::find(gameObjects.begin(), gameObjects.end(), &gameObject);
			if (position != gameObjects.end())
			{
				gameObjects.erase(position);
				globalGameObjects.push_back(&gameObject);
				return true;
			}
			else return false;
		}

		static bool DestroyGameObject(GameObject& gameObject) { DestroyObject(gameObject); }
		static bool DestroyObject(GameObject& gameObject) {
			auto position = std::find(gameObjects.begin(), gameObjects.end(), &gameObject);
			if (position != gameObjects.end())
			{
				gameObjects.erase(position);
				delete &gameObject;
				return true;
			}
			else return false;
		}

		friend class Application;
	};
}

