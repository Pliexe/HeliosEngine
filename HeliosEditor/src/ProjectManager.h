#pragma once

#include <iostream>
#include <filesystem>

namespace Helios {
	namespace Project {
		inline const std::filesystem::path GetProjectPath();
		inline const std::filesystem::path GetAssetsPath();

		void AttemptLoad();
		void SaveSceneDialog();
		void OpenSceneDialog();

		void SaveScene(bool as_new = false);
		void SetLastScenePath(std::filesystem::path scene_path);
		const std::filesystem::path GetLastScenePath();
		void TryLoad(std::filesystem::path path = Project::GetLastScenePath());
		void LoadSampleScene();
	}
}