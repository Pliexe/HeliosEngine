/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "SceneManager.h"

void Wallnut::SceneManager::Render(Graphics& graphics)
{
}

void Wallnut::SceneManager::Update()
{
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

bool  Wallnut::SceneManager::LoadScene(std::wstring name)
{
	SceneManager& sceneManager = SceneManager::getInstance();

	auto pos = sceneManager.scenes.find(name);
	if (pos == sceneManager.scenes.end()) return false;
	else {
		if(sceneManager.currentScene) sceneManager.currentScene->Unload();

		auto scene = pos->second;

		scene->Init();

		sceneManager.currentScene = scene;

		return true;
	}
}
