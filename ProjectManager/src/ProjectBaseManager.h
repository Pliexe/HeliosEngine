#pragma once

#include <filesystem>
#include "HeliosEditor_Macros.h"

#define MAX_PROJECT_NAME 50

#ifdef DLL_BUILD
#define PROJECT_MANAGER_API extern "C" __declspec(dllexport)
#else
#define PROJECT_MANAGER_API extern "C" __declspec(dllimport)
#endif

#define PROJECT_MANAGER_EXIT_OK								 0
#define PROJECT_MANAGER_EXIT_INVALID_PATH					-1
#define PROJECT_MANAGER_EXIT_INVALID_PROJECT				-2
#define PROJECT_MANAGER_EXIT_NO_READ_WRITE_PERMISSIONS		-3
#define PROJECT_MANAGER_EXIT_PROJECT_ALREADY_EXISTS			-4
#define PROJECT_MANAGER_EXIT_BAD_FILE						-5
#define PROJECT_MANAGER_EXIT_INVALID_YAML					-6
#define PROJECT_MANAGER_EXIT_PATH_MISSING					-7

namespace Helios {
	namespace Project {

		PROJECT_MANAGER_API const int GetMaxProjectName();
		PROJECT_MANAGER_API int IsValidProject(const char* path);
		PROJECT_MANAGER_API int CreateNewProject(const char* path);
		PROJECT_MANAGER_API int CreateStartupConfig(std::filesystem::path path);
		PROJECT_MANAGER_API void SerializeStartupConfig(const char* path, StartupConfig& config);
		PROJECT_MANAGER_API StartupConfig& DeserializeStartupConfig(const char* path);
	}
}