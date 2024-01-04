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
		std::unordered_set<std::filesystem::path> m_SelectedFiles;

		bool looping_cache = false;
		bool loop_next_time = false;

		GroupStyle m_CurrentGroupingStyle = GroupStyle::List;

		std::filesystem::path m_CurrentRenamePath;
		char m_CurrentName[260];
		bool m_open_rename_popup = false;
		bool m_is_renaming = false;
		bool m_focus_on_input = false;
		bool m_open_already_exists_popup = false;

		void Refresh()
		{
			if (looping_cache) {
				loop_next_time = true;
				return;
			}

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
		void RenameInputField();
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