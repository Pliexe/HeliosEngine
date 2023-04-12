/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "EditorCamera.h"
#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Helios/Core/Base.h"
#include "Helios/Graphics/Graphics.h"
#include "SceneCamera.h"

namespace Helios {
	class Scene;
	class Camera;

	class HELIOS_API SceneRegistry
	{
	public:
		static void Unregister(std::filesystem::path path);
		static void Register(std::filesystem::path path);
		static bool LoadScene(std::filesystem::path path);
		static GameObject GetPrimaryCamera();
		static void OnRuntimeUpdate();
		static void OnEditorUpdate();

		static void OnRuntimeRender();
		static void OnEditorRender(SceneCamera camera);
		static void OnEditorRender(EditorCamera camera);
		static inline Ref<Scene> get_current_scene() { return m_activeScene; }
		static Ref<Scene> create_temporary_scene();

	private:

		static std::map<std::filesystem::path, Ref<Scene>> m_scenes;
		// First entry is global scene
		static Ref<Scene> m_activeScene;

		static std::vector<std::future<void>> m_asyncTasks;

		extern friend class GameEngine;


	//	static std::map<std::string, Ref<Scene>> scenes;
	//	static Ref<Scene> currentScene;
	//	static Ref<Scene> loadQueue;

	//	//inline Scene* getCurrentScene() const { return currentScene; }

	//	static void CheckQueue();

	//public:

	//	inline static const WeakRef<Scene>& GetCurrentScene() { return currentScene; }
	//	inline static bool HasCurrentScene() { return currentScene != nullptr; }

	//	SceneRegistry() = delete;
	//	SceneRegistry(SceneRegistry const&) = delete;
	//	void operator=(SceneRegistry const&) = delete;

	//	static void Render(Graphics& graphics);
	//	static void Update();

	//	static const WeakRef<Scene>& AddScene(std::string name, std::function<void(Scene&)> callback);
	//	static const WeakRef<Scene>& AddScene(std::string name);

	//	static bool LoadScene(std::string name);

	//	friend class Application;
	//	friend class GameObject;
	//	friend class TransformComponent;

	//	friend class GameEngine;

	};
}

