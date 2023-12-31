/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once

#include "EditorCamera.h"
#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Helios/Core/Base.h"
#include <Helios/Graphics/Framebuffer.h>

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
		
		static bool LoadEmptyScene(std::string name, Mode mode = Mode::Single);
		static bool LoadScene(std::filesystem::path path, Mode mode = Mode::Single);
		static bool LoadScene(std::string name, std::filesystem::path path, Mode mode = Mode::Single);
		static bool LoadScene(std::string name, Mode mode = Mode::Single);

		static bool UnloadScene(std::string name);
		
		static Entity GetPrimaryCamera();
		static void OnRuntimeUpdate();
		static void OnEditorUpdate();

		static void OnRuntimeRender();
		static void OnEditorRender(SceneCamera camera);
		static void OnEditorRender(EditorCamera camera);

		inline static std::vector<Ref<Scene>>& GetActiveScenes() { return m_activeScenes; }
		static WeakRef<Scene> GetSceneByIndex(uint32_t scene_index);

	private:
		
		inline static std::vector<Ref<Scene>> m_activeScenes;
		inline static std::unordered_map<std::string, std::filesystem::path> m_scenePaths;

		inline static std::vector<std::future<void>> m_asyncTasks;
	};
}

