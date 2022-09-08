/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "SceneManager.h"
#include "Helios/Scene/GameObject.h"
#include "Helios/GameObjects/Components/Camera.h"

namespace Helios {
	Ref<Scene> SceneManager::loadQueue;
	Ref<Scene> SceneManager::currentScene;
	std::map<std::string, Ref<Scene>> SceneManager::scenes;

	void SceneManager::Render(Graphics& graphics)
	{
		/*auto currentCamera = currentScene->currentCamera;
		if (currentCamera) {
			graphics.m_renderTarget2D->Clear(currentCamera->backgroundColor);

			for (auto& gm : GameObject::gameObjects)
			{
				if (gm->IsActive())
					for (auto& oc : gm->components)
						if (oc->IsActive())
							oc->Render(graphics);
			}
		}*/

		if(currentScene) currentScene->RenderScene();
	}

	void SceneManager::Update()
	{
		/*for (auto& gm : GameObject::gameObjects)
		{
			if (gm->IsActive())
				for (auto& oc : gm->components)
					if (oc->IsActive())
						oc->Update();
		}*/
	}

	const WeakRef<Scene>& SceneManager::AddScene(std::string name, std::function<void(Scene&)> onInitialization)
	{
		const WeakRef<Scene>& scene = AddScene(name);
		scene.lock()->initCallback = onInitialization;
		return scene;
	}

	const WeakRef<Scene>& SceneManager::AddScene(std::string name)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		scenes.insert({ name, scene });
		return scene;
	}

	bool SceneManager::LoadScene(std::string name)
	{
		auto pos = scenes.find(name);
		if (pos == scenes.end()) return false;
		else {
			loadQueue = pos->second;
			if (currentScene == NULL) CheckQueue();
			return true;
		}
	}

	void SceneManager::CheckQueue()
	{
		if (loadQueue) {
			if (currentScene) currentScene->Unload();
			currentScene = loadQueue;
			loadQueue = NULL;

			currentScene->Init();

			if (currentScene->currentCamera == NULL) GameObject::CreateMainCamera();
		}
	}

}