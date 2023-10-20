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
					std::cout << "Opening directory: " << (m_CurrentDirectory / path.filename()).generic_string() << " | New path: " << m_CurrentDirectory.generic_string() << std::endl;
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
		ImGui::PushID(path.generic_string().c_str());

		bool is_directory = std::filesystem::is_directory((m_CurrentDirectory / path));

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
				ImGui::Image(is_directory ? HeliosEditor::ICON_FOLDER->GetTextureID() : GetFileIcon(path), ImVec2(64, 64));
				ImGui::Text(path.filename().string().c_str());
				break;
			}
			case GroupStyle::Tiles:
			{
				ImGui::Image(is_directory ? HeliosEditor::ICON_FOLDER->GetTextureID() : GetFileIcon(path), ImVec2(64, 64));
				ImGui::SameLine();
				ImGui::Text(path.filename().string().c_str());
				break;
			}
		}

		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
		{
			if (is_directory)
			{
				m_CurrentDirectory = std::filesystem::relative((m_CurrentDirectory / path), Project::GetProjectPath());
				std::cout << "Opening directory: " << (m_CurrentDirectory / path).generic_string() << " | New path: " << m_CurrentDirectory.generic_string() << std::endl;
				Refresh();
			}
		}

		ImGui::PopID();
	}

	void ExplorerPanel::DisplayFile(std::filesystem::path file)
	{
		ImGui::PushID(file.generic_string().c_str());
		switch (m_CurrentGroupingStyle)
		{
			case GroupStyle::List:
			{
				ImGui::Image(GetFileIcon(file), ImVec2(16, 16));
				ImGui::SameLine();
				ImGui::Text(file.filename().string().c_str());
				break;
			}
		}

		HandleDisplayInteractions(file, false);
		
		ImGui::PopID();
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

			bool tmp = ImGui::IsWindowHovered();

			ImGui::Columns(1);

			ImGui::Text(tmp ? "Hovered" : "Not Hovered");
			ImGui::Text(ImGui::IsAnyItemHovered() ? "Any Item Hovered" : "No Item Hovered");

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

				if (ImGui::MenuItem("Refresh"))
				{
					Refresh();
				}

				ImGui::EndPopup();
			}

			ImGui::End();
		}
		ImGui::PopStyleVar();
	}
}
