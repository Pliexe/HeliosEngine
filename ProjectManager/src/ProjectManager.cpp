#include "ProjectManager.h"

#include <filesystem>
#include <fstream>

#include "yaml-cpp/yaml.h"

#ifdef __WIN32
#include <unistd.h>
#else
#include <io.h>
//#include <Windows.h>
#define access _access
#endif

namespace Helios {
	namespace Project {

		const int GetMaxProjectName() { return MAX_PROJECT_NAME; }

		bool is_file_writable(const std::filesystem::path& file_path)
		{
			FILE* file_handle;
			errno_t file_open_error;
			if ((file_open_error = fopen_s(&file_handle, (file_path / "test").string().c_str(), "a")) != 0)
			{
				return false;
			}

			fclose(file_handle);
			return true;
		}

		int IsValidProject(const char* path)
		{
			if (!std::filesystem::is_directory(path)) return PROJECT_MANAGER_EXIT_INVALID_PATH;
			if (!is_file_writable(path)) return PROJECT_MANAGER_EXIT_NO_READ_WRITE_PERMISSIONS;
			if (!std::filesystem::exists((std::filesystem::path(path) / "Settings" / "Version.txt"))) return PROJECT_MANAGER_EXIT_INVALID_PROJECT;
				
			return false;
		}

#include <Windows.h>

		int CreateNewProject(const char* path)
		{
			if (std::filesystem::exists(path))
			{
				if (!std::filesystem::is_directory(path))
					return PROJECT_MANAGER_EXIT_INVALID_PATH;
			}
			else
				std::filesystem::create_directory(path);

			std::filesystem::path settings_folder = (std::filesystem::path(path) / "Settings");
			if (std::filesystem::exists(settings_folder))
			{
				if (!std::filesystem::is_directory(settings_folder))
					return PROJECT_MANAGER_EXIT_INVALID_PATH;
			}
			else
				std::filesystem::create_directory(settings_folder);


			std::filesystem::path version_file = (std::filesystem::path(path) / "Settings" / "Version.txt");
			if (std::filesystem::exists(version_file)) return PROJECT_MANAGER_EXIT_PROJECT_ALREADY_EXISTS;

			std::ofstream out(version_file);

			if (!out) return PROJECT_MANAGER_EXIT_BAD_FILE;

			out << std::string(VERSION_FULL);
			out.close();

			int code;

			if((code = CreateStartupConfig(path)) < 0) return code;

			return PROJECT_MANAGER_EXIT_OK;
		}

		int CreateStartupConfig(std::filesystem::path path)
		{
			try {
				StartupConfig config;
				config.game_name = path.filename().string();
				SerializeStartupConfig((path / "Settings" / "Startup.config").string().c_str(), config);
				return PROJECT_MANAGER_EXIT_OK;
			}
			catch (int code) {
				return code;
			}
		}

		void SerializeStartupConfig(const char* path, StartupConfig& config)
		{
			YAML::Emitter out;

			out << YAML::BeginMap;

			out << YAML::Key << "Window_Height" << YAML::Value << config.window_height;
			out << YAML::Key << "Window_Width" << YAML::Value << config.window_width;
			out << YAML::Key << "IsFullscreen" << YAML::Value << config.is_fullscreen;
			out << YAML::Key << "GameName" << YAML::Value << config.game_name;

			out << YAML::EndMap;

			std::ofstream fout(path);
			if(!fout.is_open())
				throw PROJECT_MANAGER_EXIT_BAD_FILE;
			fout << out.c_str();
			fout.close();
		}

		StartupConfig& DeserializeStartupConfig(const char* path)
		{
			try {
				StartupConfig config;
				auto in = YAML::LoadFile(path);
				if (in["Window_Height"]) config.window_height = in["Window_Height"].as<int>();
				if (in["Window_Width"]) config.window_width = in["Window_Width"].as<int>();
				if (in["IsFullscreen"]) config.is_fullscreen = in["IsFullscreen"].as<bool>();
				if (in["GameName"]) config.game_name = in["GameName"].as<std::string>();
				return config;
			}
			catch (YAML::BadFile) {
				StartupConfig config;
				config.game_name = std::filesystem::path(path).parent_path().parent_path().string();
				SerializeStartupConfig(path, config);
				return config;
			}
		}
	}
}
