#pragma once

#include "pch.h"

#include <iostream>
#include <filesystem>
#include <functional>

namespace Helios {
	namespace Project {
		struct ProjectSettings
		{
			std::string name;
			std::string lastScene;

			void Serialize();
			void Deserialize();
		};

		const std::filesystem::path GetProjectPath();
		const std::filesystem::path GetAssetsPath();
		const ProjectSettings& GetProjectSettings();
		
		bool HasProjectFile(std::filesystem::path path);

		void CreateProject(std::filesystem::path path, std::function<void(std::string,float)> progressCb);
		void LoadProject(std::filesystem::path path, std::function<void(std::string, float)> progressCb);

		void AttemptLoad();
		void SaveSceneDialog();
		void OpenSceneDialog();

		void SaveScene(bool as_new = false);
		void SetLastScenePath(std::filesystem::path scene_path);
		const std::filesystem::path GetLastScenePath();
		void TryLoad(std::filesystem::path path = Project::GetLastScenePath());
		void LoadSampleScene();

		void LoadAssets();
	}
}
