#include "ExplorerPanel.h"
#include <imgui.h>

#include "Application.h"
#include "Helios/Resources/Texture.h"

namespace Helios::Editor {
	ImTextureID GetFileIcon(const std::filesystem::path& file)
	{
		if (!file.has_extension()) return HeliosEditor::ICON_FILE_UNKNOWN->GetTextureID();

		std::string ext = file.extension().string();
		
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" || ext == ".bmp")
		{
			return HeliosEditor::ICON_FILE_IMAGE->GetTextureID();
		}
		else if (ext == ".cpp")
		{
			return HeliosEditor::ICON_FILE_CPP->GetTextureID();
		}
		else if (ext == ".c")
		{
			return HeliosEditor::ICON_FILE_C->GetTextureID();
		}
		else if (ext == ".h")
		{
			return HeliosEditor::ICON_FILE_H->GetTextureID();
		}
		else if (ext == ".hpp")
		{
			return HeliosEditor::ICON_FILE_HPP->GetTextureID();
		}
		else if (ext == ".txt")
		{
			return HeliosEditor::ICON_FILE_TXT->GetTextureID();
		}
		else if (ext == ".ttf" || ext == ".otf")
		{
			return HeliosEditor::ICON_FILE_FONT->GetTextureID();
		}
		else if (ext == ".scene")
		{
			return HeliosEditor::ICON_FILE_SCENE->GetTextureID();
		}
		else
		{
			return HeliosEditor::ICON_FILE_UNKNOWN->GetTextureID();
		}
	}

	void ExplorerPanel::HandleDisplayInteractions(std::filesystem::path path, bool is_directory)
	{
		if (ImGui::BeginPopup("invalid_path"))
		{
			ImGui::Text("Invalid path: %s", (m_CurrentDirectory / path).generic_string().c_str());
			ImGui::Text("clicked path: %s", path.generic_string().c_str());
			ImGui::EndPopup();
		}

		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
		{
			if (std::filesystem::exists((m_CurrentDirectory / path)))
			{
				if (std::filesystem::is_directory((m_CurrentDirectory / path.filename())))
				{
					m_CurrentDirectory /= path.filename();
					std::cout << "Opening directory: WHAT " << (m_CurrentDirectory / path.filename()).generic_string() << " | New path: " << m_CurrentDirectory.generic_string() << std::endl;
					Refresh();
				}
				else
				{
					ImGui::OpenPopup("invalid_path");
					std::cout << "Opening file: " << (m_CurrentDirectory / path.filename()).generic_string() << std::endl;
					std::cout << "Opening file: " << (m_CurrentDirectory / path).lexically_normal().generic_string() << std::endl;
					std::cout << "Opening file TEST 2: " << (m_CurrentDirectory / path).generic_string() << std::endl;
				}
			}
		}
	}

	void ExplorerPanel::DisplayDirectoryOrFile(std::filesystem::path path)
	{
		static std::filesystem::path hoveredPath;

		bool hovered = !ImGui::IsWindowHovered() && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && hoveredPath == path;

		//ImGui::PushID(path.generic_string().c_str());

		float size = 0.f;

		if (m_SelectedFiles.contains(path))
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, .5f));
			hovered = true;
		}
		else if (hovered)
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, .3f));
		//ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
		//ImGui::SetNextWindowSize(ImVec2(16, 16), ImGuiCond_FirstUseEver);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		switch (m_CurrentGroupingStyle)
		{
		/*case Helios::Editor::ExplorerPanel::GroupStyle::List:
			break;*/
		case Helios::Editor::ExplorerPanel::GroupStyle::Grid:
		{
			ImVec2 size = ImGui::CalcTextSize(path.filename().string().c_str(), NULL, false, 64.f);
			ImGui::BeginChild(path.generic_string().c_str(), ImVec2(64, 64 + size.y + 15), flags);
			break;
		}
		case Helios::Editor::ExplorerPanel::GroupStyle::Tiles:
		{
			//ImVec2 size = ImGui::CalcTextSize(path.filename().string().c_str(), NULL, false, size);
			ImGui::BeginChild(path.generic_string().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 64), flags);
			break;
		}
		case Helios::Editor::ExplorerPanel::GroupStyle::List:
		{
			ImGui::BeginChild(path.generic_string().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 16), flags);
			break;
		}
		default:
			ImGui::BeginChild(path.generic_string().c_str());
			break;
		}

		auto imgpos = ImGui::GetCursorPos();

		bool is_directory = std::filesystem::is_directory((m_CurrentDirectory / path));
#define MAX_PATH 256

		switch (m_CurrentGroupingStyle)
		{
			case GroupStyle::List:
			{
				ImGui::Image(is_directory ? HeliosEditor::ICON_FOLDER->GetTextureID() : GetFileIcon(path), ImVec2(16, 16));
				ImGui::SameLine();
				ImGui::Text(path.filename().string().c_str());
				break;
			}
			case GroupStyle::Grid:
			{
				ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 64.0f);
				ImGui::Image(is_directory ? HeliosEditor::ICON_FOLDER->GetTextureID() : GetFileIcon(path), ImVec2(64, 64));
				ImGui::Text(path.filename().string().c_str());
				ImGui::PopTextWrapPos();
				break;
			}
			case GroupStyle::Tiles:
			{
				ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
				ImGui::Image(is_directory ? HeliosEditor::ICON_FOLDER->GetTextureID() : GetFileIcon(path), ImVec2(64, 64));
				ImGui::SameLine();
				ImGui::Text(path.filename().string().c_str());
				ImGui::PopTextWrapPos();
				break;
			}
		}

		if (ImGui::BeginPopupContextWindow((const char*)0,
			ImGuiPopupFlags_MouseButtonRight |
			ImGuiPopupFlags_NoOpenOverExistingPopup
		))
		{
			if (ImGui::MenuItem("Open In Explorer"))
			{
				std::string params = "/select,\"" + (Project::GetProjectPath() / m_CurrentDirectory).string() + "\"";
#if defined(HELIOS_PLATFORM_WINDOWS)
				ShellExecuteA(NULL, "open", "explorer.exe", params.c_str(), NULL, SW_NORMAL);
#elif defined(HELIOS_PLATFORM_LINUX)
				std::string command = "xdg-open " + params;
				system(command.c_str());
#endif
			}

			if (ImGui::MenuItem("Refresh"))
			{
				Refresh();
			}

			if (ImGui::MenuItem("Rename"))
			{
				m_CurrentRenamePath = path;
				std::strncpy(m_CurrentName, path.filename().string().c_str(), 260);
				m_open_rename_popup = true;
			}

			ImGui::EndPopup();
		}

		ImGui::EndChild();

		if (ImGui::IsItemHovered())
		{
			hoveredPath = path;
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (is_directory)
				{
					m_CurrentDirectory = std::filesystem::relative((m_CurrentDirectory / path), Project::GetProjectPath());
					std::cout << " Opening directory: " << (m_CurrentDirectory / path).generic_string() << " | New path: " << m_CurrentDirectory.generic_string() << std::endl;
					Refresh();
				}
			}
			else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				if (ImGui::IsKeyDown(ImGuiKey_RightShift) || ImGui::IsKeyDown(ImGuiKey_LeftShift))
				{
					if (m_SelectedFiles.size() > 0) {
						// Select all files from the last selected item to the current one
						bool shouldSelect = false;
						bool done = false;
						// first in set = last selected
						std::filesystem::path lastSelected = *(m_SelectedFiles.begin());
						// iterate over the cached directories
						for (const auto& cachedDir : m_CachedDirectories) {
							if (cachedDir == lastSelected || cachedDir == path) {
								if (shouldSelect) {
									done = true;
									break;
								}
								shouldSelect = !shouldSelect;
							}

							if (shouldSelect) {
								m_SelectedFiles.insert(cachedDir);
							}
						}

						if (!done) {
							// iterate over the cached files
							for (const auto& cachedFile : m_CachedFiles) {
								if (cachedFile == lastSelected || cachedFile == path) {
									if (shouldSelect) break;
									shouldSelect = !shouldSelect;
								}

								if (shouldSelect) {
									m_SelectedFiles.insert(cachedFile);
								}
							}
						}
					}
				}
				else if (!(ImGui::IsKeyDown(ImGuiKey_RightCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)))
				{
					m_SelectedFiles.clear();
				}
				m_SelectedFiles.insert(path);
			}
		}

		if (hovered)
		{
			ImGui::PopStyleColor();
		}

		//ImGui::PopID();
	}

	void ExplorerPanel::OnUpdate()
	{
		if (IsCacheEmpty()) Refresh();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin(GetName().c_str(), &m_window_open, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				// Show path and button per directory in depth

				std::filesystem::path tempPath = "";

				for (auto& directory : m_CurrentDirectory)
				{
					tempPath /= directory;
					if (ImGui::Button(directory.filename().string().c_str()))
					{
						m_CurrentDirectory = tempPath;
						Refresh();
						break;
					}
					ImGui::SameLine();
					ImGui::Text(">");
					ImGui::SameLine();
				}


				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(HeliosEditor::ICON_UI_REFRESH_WHITE->GetTextureID(), ImVec2(16, 16));
				ImGui::PopStyleColor();

				if (ImGui::BeginMenu("View"))
				{
					if (ImGui::MenuItem((const char*)u8"\ue802 List"))
					{
						m_CurrentGroupingStyle = GroupStyle::List;
					}
					if (ImGui::MenuItem((const char*)u8"\ue801 Grid"))
					{
						m_CurrentGroupingStyle = GroupStyle::Grid;
					}
					if (ImGui::MenuItem((const char*)u8"\ue803 Tiles"))
					{
						m_CurrentGroupingStyle = GroupStyle::Tiles;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

				/*if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Refresh"))
				{
					Refresh();
				}
				ImGui::EndPopup();
			}*/

			switch (m_CurrentGroupingStyle)
			{
			case GroupStyle::Grid:
				ImGui::Columns(std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / 80.f)), nullptr, false);
				break;
			default:
				ImGui::Columns(2, nullptr, false);
				break;
			}

			looping_cache = true;

			for (auto& dir : m_CachedDirectories)
			{
				DisplayDirectoryOrFile(dir);
				ImGui::NextColumn();
			}
			for (auto& file : m_CachedFiles)
			{
				DisplayDirectoryOrFile(file);
				ImGui::NextColumn();
			}

			looping_cache = false;

			if (loop_next_time) {
				Refresh();
				loop_next_time = false;
			}

			bool tmp = ImGui::IsWindowHovered();

			ImGui::Columns(1);

			ImGui::Text(tmp ? "Hovered" : "Not Hovered");
			ImGui::Text(ImGui::IsAnyItemHovered() ? "Any Item Hovered" : "No Item Hovered");

			// Whole window ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)
			// Only window without children windows ImGui::IsWindowHovered()

			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				{
					m_SelectedFiles.clear();
				}
			}
			if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
			{
				m_SelectedFiles.clear();
			}

			if (ImGui::BeginPopupContextWindow((const char*)0, 
				ImGuiPopupFlags_NoOpenOverItems |
				ImGuiPopupFlags_MouseButtonRight |
				ImGuiPopupFlags_NoOpenOverExistingPopup
			))
			{
				if (ImGui::BeginMenu("New"))
				{
					if (ImGui::MenuItem("Folder"))
					{
						std::filesystem::create_directory(Project::GetAssetsPath() / "New Folder");
						Refresh();
					}
					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Open In Explorer"))
				{
					std::string params = "/select,\"" + (Project::GetProjectPath() / m_CurrentDirectory).string() + "\"";
#if defined(HELIOS_PLATFORM_WINDOWS)
					ShellExecuteA(NULL, "open", "explorer.exe", params.c_str(), NULL, SW_NORMAL);
#elif defined(HELIOS_PLATFORM_LINUX)
					std::string command = "xdg-open " + params;
					system(command.c_str());
#endif
				}

				if (ImGui::MenuItem("Refresh"))
				{
					Refresh();
				}

				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos(ImGui::GetWindowPos(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("fileRename", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::Text("Renaming %s", std::filesystem::is_directory(m_CurrentRenamePath) ? "directory" : "file");
				ImGui::InputTextMultiline("##rename", m_CurrentName, 256, ImVec2(400, ImGui::GetTextLineHeight() * 5), ImGuiInputTextFlags_EnterReturnsTrue);

				ImGui::Button("Rename");
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
			if (m_open_rename_popup) {
				ImGui::OpenPopup("fileRename");
				m_open_rename_popup = false;
			}

			ImGui::End();
		}
		ImGui::PopStyleVar();
	}
}
