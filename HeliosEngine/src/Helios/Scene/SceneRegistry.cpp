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

	void SceneRegistry::OnRuntimeRender(std::vector<Ref<Framebuffer>> frameBuffers)
	{
		HL_PROFILE_BEGIN("Scene Render");
		auto cam = GetPrimaryCamera();
		if (cam.IsNull()) return;

		auto camt = cam.GetComponent<CameraComponent>();

		frameBuffers[0]->Bind();
		switch (camt.background_mode)
		{
		case CameraComponent::BackgroundMode::None: break;
		case CameraComponent::BackgroundMode::SolidColor:
		{
			frameBuffers[0]->ClearBuffer(0u, camt.clear_color);
			break;
		}
		case CameraComponent::BackgroundMode::Skybox:
		{
			frameBuffers[0]->ClearBuffer(0u, Color::Black);
			break;
		}
		}
		frameBuffers[0]->ClearDepthStencil();

		TransformComponent worldTransform = Transform(cam, cam.GetContainer()).GetWorldTransformCache();

		Matrix4x4 projection = Camera::GetViewProjection(worldTransform, camt, frameBuffers[0]->GetSize());

		frameBuffers[0]->Unbind();

		for (auto& scene : m_activeScenes)
		{
			scene->RenderScene(frameBuffers[0], worldTransform, projection);

			scene->PerformCleanupAndSync();
		}


		HL_PROFILE_END();
	}

	void SceneRegistry::OnEditorRender(Ref<Framebuffer>& framebuffer, EditorCamera camera)
	{
		HL_PROFILE_BEGIN("Editor Scene Render");

		for (auto& scene : m_activeScenes)
		{
			scene->RenderScene(framebuffer, camera); 

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
