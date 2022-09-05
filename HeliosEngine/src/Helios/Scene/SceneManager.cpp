/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "SceneManager.h"
#include "Helios/GameObjects/GameObject.h"
#include "Helios/GameObjects/Components/Camera.h"

Helios::Scene* Helios::SceneManager::loadQueue = NULL;
Helios::Scene* Helios::SceneManager::currentScene = NULL;
std::map<std::wstring, Helios::Scene*> Helios::SceneManager::scenes;

void Helios::SceneManager::Render(Graphics& graphics)
{
	auto currentCamera = currentScene->currentCamera;
	if (currentCamera) {
		graphics.m_renderTarget2D->Clear(currentCamera->backgroundColor);

		for (auto& gm : GameObject::gameObjects)
		{
			if(gm->IsActive())
			for (auto& oc : gm->components)
				if (oc->IsActive())
				oc->Render(graphics);
		}
	}
}

void Helios::SceneManager::Update()
{
	for (auto& gm : GameObject::gameObjects)
	{
		if (gm->IsActive())
		for (auto& oc : gm->components)
			if (oc->IsActive())
			oc->Update();
	}
}

Helios::Scene& Helios::SceneManager::AddScene(std::wstring name, Scene* scene)
{
	scenes.insert({ name, scene });
	return *scene;
}

Helios::Scene& Helios::SceneManager::AddScene(std::wstring name, std::function<void(Scene&)> onInitialization)
{
	Scene& scene = AddScene(name);
	scene.initCallback = onInitialization;
	return scene;
}

Helios::Scene& Helios::SceneManager::AddScene(std::wstring name)
{
	Scene* scene = new Scene();
	scenes.insert({ name, scene });
	return *scene;
}

bool Helios::SceneManager::LoadScene(std::wstring name)
{
	auto pos = scenes.find(name);
	if (pos == scenes.end()) return false;
	else {
		loadQueue = pos->second;
		if (currentScene == NULL) CheckQueue();
		return true;
	}
}

void Helios::SceneManager::CheckQueue()
{
	if (loadQueue) {
		if (currentScene) currentScene->Unload();
		currentScene = loadQueue;
		loadQueue = NULL;

		currentScene->Init();

		if (currentScene->currentCamera == NULL) GameObject::CreateMainCamera();
	}
}
