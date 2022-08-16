/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "GameObject.h"

#include "ObjectComponent.h"
#include "Components/Camera.h"
#include "ObjectComponent.h"

std::vector<Wallnut::GameObject*> Wallnut::GameObject::gameObjects;
std::vector<Wallnut::GameObject*> Wallnut::GameObject::globalGameObjects;
std::vector<Wallnut::ObjectComponent*> Wallnut::GameObject::components;

//Wallnut::Transform Wallnut::GameObject::transform = Wallnut::Transform();

void Wallnut::GameObject::AddComponent(ObjectComponent& oc) {
	oc.gameObject = this;
	components.push_back(&oc);
}

void Wallnut::GameObject::AddComponent(ObjectComponent* oc) {
	oc->gameObject = this;
	oc->transform = &this->transform;
	components.push_back(oc);
}

Wallnut::GameObject& Wallnut::GameObject::CreateMainCamera(Vector2D position) {
	GameObject* obj = new GameObject();
	obj->transform.setPosition(position);
	Camera* cam = new Camera();
	obj->AddComponent(cam);
	SceneManager::getInstance().currentScene->currentCamera = cam;
	return *obj;
}

Wallnut::GameObject& Wallnut::GameObject::CreateCamera(Vector2D position)
{
	GameObject* obj = new GameObject();
	obj->transform.setPosition(position);
	Camera* cam = new Camera();
	obj->AddComponent(cam);
	return *obj;
}

bool Wallnut::GameObject::Destroy(ObjectComponent* oc)
{
	std::vector<ObjectComponent*>& components = oc->gameObject->components;
	auto pos = std::find(components.begin(), components.end(), oc);
	if (pos != components.end())
	{
		//deletionQueueComponents.insert(std::make_pair(oc->gameObject, static_cast<int>(pos - components.begin())));
		return true;
	}
	else return false;
}

bool Wallnut::GameObject::DestroyObject(GameObject& gameObject)
{
	if (gameObject.isGlobal) {
		auto position = std::find(globalGameObjects.begin(), globalGameObjects.end(), &globalGameObjects);
		if (position != globalGameObjects.end())
		{
			deletionQueueGameObjects.insert(position - globalGameObjects.begin());
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