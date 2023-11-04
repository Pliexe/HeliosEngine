#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <imgui.h>

enum class FileType {
	None,
	Cpp,
	Hpp,
	C,
	H,
	Folder
};

std::string GetFileExtension(FileType type);
void CreateFileFromType(std::filesystem::path path, FileType type, std::string name);
void ProjectExplorer_RightClickMenu(std::filesystem::path path, int count, bool is_root);
void SetCurrentPath(std::filesystem::path projectPath);
void ProjectExplorerWindow(std::filesystem::path projectPath);

ImTextureID GetFileIcon(std::filesystem::path file);
ImTextureID GetFileIcon(FileType type);