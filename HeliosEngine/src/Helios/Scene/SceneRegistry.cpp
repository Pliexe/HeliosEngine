/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#include "SceneRegistry.h"

#include "EditorCamera.h"
#include "Scene.h"
#include "Helios/Core/Profiler.h"
#include "Helios/Scripting/ScriptManager.h"
#include "Helios/Scripting/BehaviourStore.h"

namespace Helios {

	inline static std::unordered_map<std::string, std::filesystem::path> m_scene_paths;
	inline static std::vector<Ref<Scene>> m_active_scenes;
	inline static std::vector<std::future<void>> m_asyncTasks;

	void SceneRegistry::Register(const std::string& name, const std::filesystem::path& path)
	{
		m_scene_paths.emplace(name, path);
	}

	bool SceneRegistry::LoadEmptyScene(std::string name, Mode mode)
	{
		// Check if a scene with the same name already exists
		if (const auto x = m_scene_paths.find(name); x != m_scene_paths.end()) return false;

		Ref<Scene> new_scene = Scene::Create(name);
		if (mode == Mode::Single)
		{
			for (auto& scene : m_active_scenes)
			{
				scene->GetComponentsWith<entt::entity>().each([scene](auto entity)
				{
					if (scene->GetComponents().any_of<DontDestroyOnLoadComponent>(entity))
					{

					}
				});
			}

			m_active_scenes.clear();
		}
		m_active_scenes.emplace_back(new_scene);

		return true;
	}

	bool SceneRegistry::LoadScene(const std::filesystem::path& path, Mode mode)
	{
		return LoadScene(path.filename().generic_string(), path, mode);
	}

	bool SceneRegistry::LoadScene(const std::string& name, const std::filesystem::path& path, Mode mode)
	{
		Ref<Scene> scene = Scene::Create(name);
		Scene::Deserialize(path.string(), scene);
		m_active_scenes.emplace_back(scene);
		return true;
	}


	bool SceneRegistry::LoadScene(const std::string& name, Mode mode)
	{
		if (const auto x = m_scene_paths.find(name); x == m_scene_paths.end()) return false;
		else return LoadScene(name, x->second, mode);
	}
	
	bool SceneRegistry::UnloadScene(const std::string& name)
	{
		for (auto scene : m_active_scenes)
		{
			if (scene->GetName() == name)
			{
				m_active_scenes.erase(std::remove(m_active_scenes.begin(), m_active_scenes.end(), scene), m_active_scenes.end());
				return true;
			}
		}
		return false;
	}

	Entity SceneRegistry::GetPrimaryCamera()
	{
		Entity primaryCamera;

		for (auto& scene : m_active_scenes)
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

		for (auto& scene : m_active_scenes)
		{
			Scene::UpdateChildTransforms(scene);
		}

		Scripting::UpdateServices();
		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorUpdate()
	{
		HL_PROFILE_BEGIN("Scene Editor - Update");
		for (auto& scene : m_active_scenes)
		{
			Scene::UpdateChildTransforms(scene);
		}

		Scripting::UpdateEditorServices();
		HL_PROFILE_END();
	}

	void SceneRegistry::OnRuntimeRender(std::vector<Ref<Image>> frameBuffers)
	{
		HL_PROFILE_BEGIN("Scene Render");
		auto cam = GetPrimaryCamera();
		if (cam.IsNull()) return;

		auto camt = cam.GetComponent<CameraComponent>();

		// frameBuffers[0]->Bind();
		switch (camt.background_mode)
		{
		case CameraComponent::BackgroundMode::None: break;
		case CameraComponent::BackgroundMode::SolidColor:
		{
			// frameBuffers[0]->ClearBuffer(0u, camt.clear_color);
			break;
		}
		case CameraComponent::BackgroundMode::Skybox:
		{
			// frameBuffers[0]->ClearBuffer(0u, Color::Black);
			break;
		}
		}
		// frameBuffers[0]->ClearDepthStencil();

		TransformComponent worldTransform = Transform(cam, cam.GetContainer()).GetWorldTransformCache();

		Matrix4x4 projection = Camera::GetViewProjection(worldTransform, camt, frameBuffers[0]->GetSize());

		// frameBuffers[0]->Unbind();

		for (auto& scene : m_active_scenes)
		{
			// scene->RenderScene(frameBuffers[0], worldTransform, projection);

			scene->PerformCleanupAndSync();
		}


		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorRender(Ref<Image>& framebuffer, EditorCamera camera)
	{
		HL_PROFILE_BEGIN("Editor Scene Render");

		for (auto& scene : m_active_scenes)
		{
			// scene->RenderScene(framebuffer, camera); 

			scene->PerformCleanupAndSync();
		}
		HL_PROFILE_END();
	}

	std::vector<Ref<Scene>>& SceneRegistry::GetActiveScenes()
	{ return m_active_scenes; }

	WeakRef<Scene> SceneRegistry::GetSceneByIndex(uint32_t scene_index)
	{
		assert(scene_index < m_active_scenes.size());
		return m_active_scenes[scene_index];
	}

#ifdef HELIOS_EDITOR
	inline static std::vector<Ref<Scene>> m_active_scenes_backup;

  	void SceneRegistry::Backup()
	{
		for (auto& scene : m_active_scenes)
		{
			auto new_scene = Scene::Create(scene->GetName());

			new_scene->CloneFrom(scene);

			m_active_scenes_backup.emplace_back(new_scene);

			/*for (auto entity : scene->GetComponents())
			{
				auto new_entity = new_scene->CreateEntity(entity.GetName());
				*/
		}
	}

	void SceneRegistry::Restore()
	{
		std::cout << "-----------------------------------\n";
		std::cout << "STATE 1: BEH STORE SIZE: " << Scripting::GetBehaviourStore("CSharp").GetBehaviours().size() << "\n";
		for (auto& behaviour : Scripting::GetBehaviourStore("CSharp").GetBehaviours())
		{
			std::cout << "(" << behaviour.GetHandle() << ", " << (uint32_t)behaviour.GetEntity()->GetHandle() << ")" << behaviour.GetName() << " | Scene expired" << behaviour.GetEntity()->GetContainer().expired() << "\n";
		}
		std::cout << "-----------------------------------\n";
		Scripting::GetBehaviourStore("CSharp").test();
		m_active_scenes.clear();
		std::cout << "-----------------------------------\n";
		std::cout << "STATE 2: BEH STORE SIZE: " << Scripting::GetBehaviourStore("CSharp").GetBehaviours().size() << "\n";
		for (auto& behaviour : Scripting::GetBehaviourStore("CSharp").GetBehaviours())
		{
			std::cout << "(" << behaviour.GetHandle() << ", " << (uint32_t)behaviour.GetEntity()->GetHandle() << ")" << behaviour.GetName() << " | Scene expired" << behaviour.GetEntity()->GetContainer().expired() << "\n";
		}
		std::cout << "-----------------------------------\n";
		Scripting::GetBehaviourStore("CSharp").test();
		for (auto& scene : m_active_scenes_backup)
		{
			auto new_scene = Scene::Create(scene->GetName());
			new_scene->CloneFrom(scene);

			for (auto view = new_scene->GetComponentsWith<BoxCollider2D>(); auto& entity : view)
			{
				auto& body = view.get<BoxCollider2D>(entity);

				body.body = nullptr;
			}

			m_active_scenes.emplace_back(new_scene);
		}
		std::cout << "-----------------------------------\n";
		std::cout << "STATE 3: BEH STORE SIZE: " << Scripting::GetBehaviourStore("CSharp").GetBehaviours().size() << "\n";
		for (auto& behaviour : Scripting::GetBehaviourStore("CSharp").GetBehaviours())
		{
			std::cout << "(" << behaviour.GetHandle() << ", " << (uint32_t)behaviour.GetEntity()->GetHandle() << ")" << behaviour.GetName() << " | Scene expired" << behaviour.GetEntity()->GetContainer().expired() << "\n";
		}
		std::cout << "-----------------------------------\n";
		Scripting::GetBehaviourStore("CSharp").test();
		m_active_scenes_backup.clear();
		std::cout << "-----------------------------------\n";
		std::cout << "STATE 4: BEH STORE SIZE: " << Scripting::GetBehaviourStore("CSharp").GetBehaviours().size() << "\n";
		for (auto& behaviour : Scripting::GetBehaviourStore("CSharp").GetBehaviours())
		{
			std::cout << "(" << behaviour.GetHandle() << ", " << (uint32_t)behaviour.GetEntity()->GetHandle() << ")" << behaviour.GetName() << " | Scene expired" << behaviour.GetEntity()->GetContainer().expired() << "\n";
		}
		std::cout << "-----------------------------------\n";
		Scripting::GetBehaviourStore("CSharp").test();
		Scripting::ReloadAllServices();
	}
#endif

	void SceneRegistry::Shutdown()
    {
		#ifdef HELIOS_EDITOR
		m_active_scenes_backup.clear();
		#endif

		m_active_scenes.clear();
		m_asyncTasks.clear();
		m_scene_paths.clear();
    }
}
