/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "SceneRegistry.h"
#include "Scene.h"
#include "Helios/Core/Application.h"
#include "Helios/Graphics/GizmosRenderer.h"
#include "Helios/Scene/GameObject.h"
#include "Helios/Core/Profiler.h"

namespace Helios {
	Ref<Scene> SceneRegistry::m_activeScene;
	std::vector<std::future<void>> SceneRegistry::m_asyncTasks;
	std::map<std::filesystem::path, Ref<Scene>> SceneRegistry::m_scenes;

	/*Ref<Scene> SceneRegistry::loadQueue;
	Ref<Scene> SceneRegistry::currentScene;*/
	//std::map<std::string, Ref<Scene>> SceneRegistry::scenes;

	//void SceneRegistry::Render(Graphics& graphics)
	//{
	//	/*auto currentCamera = currentScene->currentCamera;
	//	if (currentCamera) {
	//		graphics.m_renderTarget2D->Clear(currentCamera->backgroundColor);

	//		for (auto& gm : GameObject::gameObjects)
	//		{
	//			if (gm->IsActive())
	//				for (auto& oc : gm->components)
	//					if (oc->IsActive())
	//						oc->Render(graphics);
	//		}
	//	}*/

	//	//if(currentScene) currentScene->RenderScene();
	//}

	//void SceneRegistry::Update()
	//{
	//	/*for (auto& gm : GameObject::gameObjects)
	//	{
	//		if (gm->IsActive())
	//			for (auto& oc : gm->components)
	//				if (oc->IsActive())
	//					oc->Update();
	//	}*/
	//}

	//const WeakRef<Scene>& SceneRegistry::AddScene(std::string name, std::function<void(Scene&)> onInitialization)
	//{
	//	const WeakRef<Scene>& scene = AddScene(name);
	//	scene.lock()->initCallback = onInitialization;
	//	return scene;
	//}

	//const WeakRef<Scene>& SceneRegistry::AddScene(std::string name)
	//{
	//	Ref<Scene> scene = CreateRef<Scene>();
	//	scenes.insert({ name, scene });
	//	return scene;
	//}

	//bool SceneRegistry::LoadScene(std::string name)
	//{
	//	auto pos = scenes.find(name);
	//	if (pos == scenes.end()) return false;
	//	else {
	//		loadQueue = pos->second;
	//		if (currentScene == NULL) CheckQueue();
	//		return true;
	//	}
	//}

	//void SceneRegistry::CheckQueue()
	//{
	//	if (loadQueue) {
	//		if (currentScene) currentScene->Unload();
	//		currentScene = loadQueue;
	//		loadQueue = NULL;

	//		currentScene->Init();

	//		if (!currentScene->IsPrimaryCameraSet()) GameObject::CreateMainCamera();
	//	}
	//}

	void SceneRegistry::Unregister(std::filesystem::path path)
	{
		m_scenes.erase(path);
	}

	void SceneRegistry::Register(std::filesystem::path path)
	{
		m_scenes.emplace(path, CreateRef<Scene>(path.filename().generic_string()));
	}

	Ref<Scene> SceneRegistry::create_temporary_scene()
	{
		return m_activeScene = CreateRef<Scene>("New Scene");
	}

	bool SceneRegistry::LoadScene(std::filesystem::path path)
	{
		std::map<std::filesystem::path, Ref<Scene>>::iterator x;
		if ((x = m_scenes.find(path)) != m_scenes.end())
		{
			m_activeScene->Shutdown();
			x->second->Init();
			Scene::Deserialize(path.string(), x->second);
			return true;
		}
		else return false;
	}

	GameObject SceneRegistry::GetPrimaryCamera()
	{
		GameObject primaryCamera;

		auto view = m_activeScene->GetComponents<Components::Camera>();
		for (auto& entity : view)
		{
			if (view.get<Components::Camera>(entity).isPrimary)
			{
				primaryCamera = { entity, m_activeScene };
				break;
			}
		}
		
		return primaryCamera;
	}

	void SceneRegistry::OnRuntimeUpdate()
	{
		HL_PROFILE_BEGIN("Scene Runtime - Update");
		Scene::UpdateChildTransforms(m_activeScene);
		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorUpdate()
	{
		HL_PROFILE_BEGIN("Scene Editor - Update");
		Scene::UpdateChildTransforms(m_activeScene);
		HL_PROFILE_END();
	}

	void SceneRegistry::OnRuntimeRender()
	{
		HL_PROFILE_BEGIN("Scene Render");
		//SceneCamera::GetProjection()
		auto cam = GetPrimaryCamera();
		if (cam.IsNull()) return;

		Matrix4x4 projection = SceneCamera::GetProjection(cam.GetComponent<Components::Transform, Components::Camera>());

		m_activeScene->RenderScene(projection);
		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorRender(SceneCamera camera)
	{
		HL_PROFILE_BEGIN("Editor Scene Render");
		m_activeScene->RenderScene(camera);
		HL_PROFILE_END();
	}
}
