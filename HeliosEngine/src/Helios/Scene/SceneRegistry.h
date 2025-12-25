/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#pragma once

#include "EditorCamera.h"
#include "Helios/Resources/Image.h"
#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Helios/Core/Base.h"

namespace Helios {
	class Scene;
	class Camera;

	class HELIOS_API SceneRegistry
	{
	public:
		enum class Mode : uint8_t { Single, Additive };
		static void Register(const std::string& name, const std::filesystem::path& path);
		
		static bool LoadEmptyScene(std::string name, Mode mode = Mode::Single);
		static bool LoadScene(const std::filesystem::path& path, Mode mode = Mode::Single);
		static bool LoadScene(const std::string& name, const std::filesystem::path& path, Mode mode = Mode::Single);
		static bool LoadScene(const std::string& name, Mode mode = Mode::Single);
		
		static bool UnloadScene(const std::string& name);
		
		static Entity GetPrimaryCamera();
		static void OnRuntimeUpdate();
		static void OnEditorUpdate();
		
		static void OnRuntimeRender(std::vector<Ref<Image>> frameBuffers);
		static void OnEditorRender(Ref<Image>& framebuffer, EditorCamera camera);

		static std::vector<Ref<Scene>>& GetActiveScenes();
		static WeakRef<Scene> GetSceneByIndex(uint32_t scene_index);

		static void Shutdown();

#ifdef HELIOS_EDITOR
		static void Backup();

		static void Restore();
#endif
	};
}

