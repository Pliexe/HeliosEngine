#pragma once

#include "BasePanel.h"
#include "ProjectManager.h"

namespace Helios::Editor {
	class ExplorerPanel : public IPanel
	{
	public:
		enum class GroupStyle
		{
			List,
			Grid,
			Tiles,
		};
	private:
		inline static std::filesystem::path m_CurrentDirectory = "Assets";

		// Cache
		std::vector<std::filesystem::path> m_CachedDirectories;
		std::vector<std::filesystem::path> m_CachedFiles;

		GroupStyle m_CurrentGroupingStyle = GroupStyle::List;

		void Refresh()
		{
			m_CachedDirectories.clear();
			m_CachedFiles.clear();

			for (const auto& entry : std::filesystem::directory_iterator(Helios::Project::GetProjectPath() / m_CurrentDirectory))
			{
				if (entry.is_directory())
					m_CachedDirectories.push_back(entry.path());
				else
					m_CachedFiles.push_back(entry.path());
			}
		}

		bool IsCacheEmpty() const
		{
			return m_CachedDirectories.empty() && m_CachedFiles.empty();
		}

		void HandleDisplayInteractions(std::filesystem::path path, bool is_directory);
		void DisplayDirectoryOrFile(std::filesystem::path path);
		void DisplayFile(std::filesystem::path file);

	public:
		ExplorerPanel()
		{
			title = "Explorer";
			m_custom_begin = true;
		}

		
		virtual void OnUpdate() override;
	};
}