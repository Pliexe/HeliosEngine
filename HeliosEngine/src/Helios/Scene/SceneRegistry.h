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
#include "Helios/Graphics/DepricatedGraphics.h"
#include "SceneCamera.h"

namespace Helios {
	class Scene;
	class Camera;

	class HELIOS_API SceneRegistry
	{
	public:
		enum class Mode
		{
			Single,
			Additive
		};
		
		static void Register(std::string name, std::filesystem::path path);
		
		static void LoadEmptyScene(std::string name);
		static bool LoadScene(std::filesystem::path path, Mode mode = Mode::Single);
		static bool LoadScene(std::string name, std::filesystem::path path, Mode mode = Mode::Single);
		static bool LoadScene(std::string name, Mode mode = Mode::Single);

		static bool UnloadScene(std::string name);
		
		static GameObject GetPrimaryCamera();
		static void OnRuntimeUpdate();
		static void OnEditorUpdate();

		static void OnRuntimeRender();
		static void OnEditorRender(SceneCamera camera);
		static void OnEditorRender(EditorCamera camera);
		static inline Ref<Scene> get_current_scene() { return m_activeScene; }
		static Ref<Scene> create_temporary_scene();

	private:
		
		static std::vector<Ref<Scene>> m_activeScenes;
		static std::unordered_map<std::string, std::filesystem::path> m_scenePaths;

		static std::vector<std::future<void>> m_asyncTasks;

		extern friend class GameEngine;
		extern friend class HeliosEditor;
	};
}

