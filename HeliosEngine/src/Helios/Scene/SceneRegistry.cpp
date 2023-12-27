/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "SceneRegistry.h"

#include "EditorCamera.h"
#include "Scene.h"
#include "Helios/Graphics/GizmosRenderer.h"
#include "Helios/Scene/Entity.h"
#include "Helios/Core/Profiler.h"

namespace Helios {
	/*Ref<Scene> SceneRegistry::loadQueue;
	Ref<Scene> SceneRegistry::currentScene;*/
	//std::map<std::string, Ref<Scene>> SceneRegistry::scenes;

	//void SceneRegistry::Render(DepricatedGraphics& graphics)
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
	
	

	void SceneRegistry::Register(std::string name, std::filesystem::path path)
	{
		m_scenePaths.emplace(name, path);
	}

	bool SceneRegistry::LoadEmptyScene(std::string name, Mode mode)
	{
		// Check if a scene with the same name already exists
		if (const auto x = m_scenePaths.find(name); x != m_scenePaths.end()) return false;

		Ref<Scene> new_scene = CreateRef<Scene>(name);
		if (mode == Mode::Single)
		{
			for (auto& scene : m_activeScenes)
			{
				scene->GetComponents().each([scene](auto entity)
				{
					if (scene->GetComponents().any_of<DontDestroyOnLoadComponent>(entity))
					{

					}
				});
			}

			m_activeScenes.clear();
		}
		m_activeScenes.emplace_back(new_scene);

		return true;
	}

	bool SceneRegistry::LoadScene(std::filesystem::path path, Mode mode)
	{
		return LoadScene(path.filename().generic_string(), path, mode);
	}

	bool SceneRegistry::LoadScene(std::string name, std::filesystem::path path, Mode mode)
	{
		Ref<Scene> scene = CreateRef<Scene>(name);
		Scene::Deserialize(path.string(), scene);
		m_activeScenes.emplace_back(scene);
		return true;
	}


	bool SceneRegistry::LoadScene(std::string name, Mode mode)
	{
		if (const auto x = m_scenePaths.find(name); x == m_scenePaths.end()) return false;
		else return LoadScene(name, x->second, mode);
	}
	
	bool SceneRegistry::UnloadScene(std::string name)
	{
		for (auto scene : m_activeScenes)
		{
			if (scene->GetName() == name)
			{
				m_activeScenes.erase(std::remove(m_activeScenes.begin(), m_activeScenes.end(), scene), m_activeScenes.end());
				return true;
			}
		}
		return false;
	}

	Entity SceneRegistry::GetPrimaryCamera()
	{
		Entity primaryCamera;

		for (auto& scene : m_activeScenes)
		{
			auto view = scene->GetComponentsWith<CameraComponent>();
			for (auto& entity : view)
			{
				if (view.get<CameraComponent>(entity).isPrimary)
				{
					primaryCamera = { entity, scene };
					break;
				}
			}
		}
		
		return primaryCamera;
	}

	void SceneRegistry::OnRuntimeUpdate()
	{
		HL_PROFILE_BEGIN("Scene Runtime - Update");

		for (auto& scene : m_activeScenes)
		{
			Scene::UpdateChildTransforms(scene);
		}
		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorUpdate()
	{
		HL_PROFILE_BEGIN("Scene Editor - Update");
		for (auto& scene : m_activeScenes)
		{
			Scene::UpdateChildTransforms(scene);
			scene->UpdatePhysics();
		}

		HL_PROFILE_END();
	}

	void SceneRegistry::OnRuntimeRender()
	{
		HL_PROFILE_BEGIN("Scene Render");
		//SceneCamera::GetViewProjection()
		auto cam = GetPrimaryCamera();
		if (cam.IsNull()) return;

		auto camt = cam.GetComponent<CameraComponent>();

		TransformComponent worldTransform = Transform(cam, cam.GetScene()).GetWorldTransformCache();

		Matrix4x4 projection = SceneCamera::GetViewProjection(worldTransform, camt);

		for (auto& scene : m_activeScenes)
		{
			scene->RenderScene(worldTransform, projection);

			scene->PerformCleanupAndSync();
		}

		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorRender(SceneCamera camera)
	{
		HL_PROFILE_BEGIN("Editor Scene Render");

		for (auto& scene : m_activeScenes)
		{
			scene->RenderScene(camera);

			scene->PerformCleanupAndSync();
		}

		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorRender(EditorCamera camera)
	{
		HL_PROFILE_BEGIN("Editor Scene Render");

		for (auto& scene : m_activeScenes)
		{
			scene->RenderScene(camera); 

			scene->PerformCleanupAndSync();
		}
		HL_PROFILE_END();
	}

	WeakRef<Scene> SceneRegistry::GetSceneByIndex(uint32_t scene_index)
	{
		assert(scene_index < m_activeScenes.size());
		return m_activeScenes[scene_index];
	}
}
