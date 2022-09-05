/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "GameObject.h"

#include "ObjectComponent.h"
#include "Components/Camera.h"
#include "ObjectComponent.h"

std::vector<Helios::GameObject*> Helios::GameObject::gameObjects;
std::vector<Helios::GameObject*> Helios::GameObject::globalGameObjects;

std::queue<Helios::GameObject*> Helios::GameObject::objectQueue;
std::queue<Helios::ObjectComponent*> Helios::GameObject::objectComponentQueue;

std::unordered_set<Helios::ObjectComponent*> Helios::GameObject::deletionQueueComponents;
std::unordered_set<Helios::ObjectComponent*> Helios::GameObject::initList;
std::unordered_set<int> Helios::GameObject::deletionQueueGlobalGameObjects;
std::unordered_set<int> Helios::GameObject::deletionQueueGameObjects;


void Helios::GameObject::CheckQueue(Graphics& graphics)
{
	for (auto& x : deletionQueueComponents)
	{
		auto pos = std::find(x->gameObject->components.begin(), x->gameObject->components.end(), x);
		if (pos != x->gameObject->components.end())
		{
			delete x->gameObject->components[pos - x->gameObject->components.begin()];
			x->gameObject->components.erase(pos);
		}
	}
	deletionQueueComponents.clear();

	for (auto& i : deletionQueueGameObjects)
	{
		delete gameObjects[i];
		gameObjects.erase(gameObjects.begin() + i);
	}
	deletionQueueGameObjects.clear();

	for (auto& i : deletionQueueGlobalGameObjects)
	{
		delete globalGameObjects[i];
		globalGameObjects.erase(globalGameObjects.begin() + i);
	}
	deletionQueueGlobalGameObjects.clear();

	while (!objectQueue.empty())
	{
		gameObjects.push_back(objectQueue.front());
		objectQueue.pop();
	}

	while (!objectComponentQueue.empty())
	{
		auto x = objectComponentQueue.front();
		x->Init(graphics);
		x->gameObject->components.push_back(x);
		objectComponentQueue.pop();
	}
}


//Helios::Transform Helios::GameObject::transform = Helios::Transform();

void Helios::GameObject::AddComponent(ObjectComponent& oc) {
	AddComponent(&oc);
}

void Helios::GameObject::AddComponent(ObjectComponent* oc) {
	oc->gameObject = this;
	oc->transform = &this->transform;
	objectComponentQueue.push(oc);
}

Helios::GameObject& Helios::GameObject::CreateMainCamera(Vector2D position) {
	GameObject* obj = new GameObject();
	obj->m_name = "MainCamera";
	obj->transform.setPosition(position);
	Camera* cam = new Camera();
	obj->AddComponent(cam);
	SceneManager::currentScene->currentCamera = cam;
	objectQueue.push(obj);
	return *obj;
}

Helios::GameObject& Helios::GameObject::CreateCamera(Vector2D position)
{
	GameObject* obj = new GameObject();
	obj->m_name = "Camera";
	obj->transform.setPosition(position);
	Camera* cam = new Camera();
	obj->AddComponent(cam);
	objectQueue.push(obj);
	return *obj;
}

void Helios::GameObject::Destroy(ObjectComponent* oc)
{
	deletionQueueComponents.insert(oc);
}

bool Helios::GameObject::DestroyObject(GameObject& gameObject)
{
	if (gameObject.isGlobal) {
		auto position = std::find(globalGameObjects.begin(), globalGameObjects.end(), &gameObject);
		if (position != globalGameObjects.end())
		{
			deletionQueueGameObjects.insert(static_cast<int>(position - globalGameObjects.begin()));
			return true;
		}
		else return false;
	}
	else {
		auto position = std::find(gameObjects.begin(), gameObjects.end(), &gameObject);
		if (position != gameObjects.end())
		{
			deletionQueueGameObjects.insert(position - gameObjects.begin());
			return true;
		}
		else return false;
	}
}