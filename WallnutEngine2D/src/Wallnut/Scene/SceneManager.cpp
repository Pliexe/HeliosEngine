/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "SceneManager.h"
#include "Wallnut/GameObjects/GameObject.h"
#include "Wallnut/GameObjects/Components/Camera.h"

Wallnut::Scene* Wallnut::SceneManager::loadQueue = NULL;

void Wallnut::SceneManager::Render(Graphics& graphics)
{
	auto currentCamera = currentScene->currentCamera;
	if (currentCamera) {
		graphics.renderTarget->Clear(currentCamera->backgroundColor);
		for (auto& x : GameObject::components)
		{
			x->Render(graphics);
		}
	}
}

void Wallnut::SceneManager::Update()
{
	for (auto& x : GameObject::components)
	{
		x->Update();
	}
}

Wallnut::Scene& Wallnut::SceneManager::AddScene(std::wstring name, Scene* scene)
{
	SceneManager::getInstance().scenes.insert({ name, scene });
	return *scene;
}

Wallnut::Scene& Wallnut::SceneManager::AddScene(std::wstring name, std::function<void(Scene&)> onInitialization)
{
	Scene& scene = AddScene(name);
	scene.initCallback = onInitialization;
	return scene;
}

Wallnut::Scene& Wallnut::SceneManager::AddScene(std::wstring name)
{
	Scene* scene = new Scene();
	SceneManager::getInstance().scenes.insert({ name, scene });
	return *scene;
}

bool Wallnut::SceneManager::LoadScene(std::wstring name)
{
	SceneManager& sceneManager = SceneManager::getInstance();

	auto pos = sceneManager.scenes.find(name);
	if (pos == sceneManager.scenes.end()) return false;
	else {
		loadQueue = pos->second;
		return true;
	}
}

void Wallnut::SceneManager::CheckQueue()
{
	if (loadQueue) {
		SceneManager& sceneManager = SceneManager::getInstance();
		if (sceneManager.currentScene) sceneManager.currentScene->Unload();
		sceneManager.currentScene = loadQueue;

		sceneManager.currentScene->Init();
		for (auto& x : GameObject::components)
			x->Init(*Graphics::instance);

		if (sceneManager.currentScene->currentCamera == NULL) GameObject::CreateMainCamera();
	}
}
