
#include "ProjectManager.h"


#include "HeliosEditor_Macros.h"

#include <filesystem>
#include <fstream>
// #include <Windows.h>
// #include <atlstr.h>

namespace Helios {
	namespace Project {

		static std::filesystem::path s_projectPath;
		static std::filesystem::path s_currentScenePath;
		static ProjectSettings s_projectSettings;

		static StartupConfig s_startupConfig = {};

		void ProjectSettings::Serialize()
		{
			YAML::Emitter out;

			out << YAML::BeginMap;

			out << YAML::Key << "Name" << YAML::Value << name;
			if (!lastScene.empty()) out << YAML::Key << "LastScene" << YAML::Value << lastScene;

			out << YAML::EndMap;

			std::ofstream fout(s_projectPath / "project.helios");
			fout << out.c_str();
			fout.close();
		}

		void ProjectSettings::Deserialize()
		{
			assert(std::filesystem::exists(s_projectPath / "project.helios"));

			YAML::Node in = YAML::LoadFile((s_projectPath / "project.helios").generic_string());

			name = in["Name"].as<std::string>();
			if (in["LastScene"].IsDefined()) lastScene = in["LastScene"].as<std::string>();
		}

		const std::filesystem::path GetProjectPath() { return s_projectPath; }
		const std::filesystem::path GetAssetsPath() { return s_projectPath / "Assets"; }

		const ProjectSettings& GetProjectSettings()
		{
			return s_projectSettings;
		}

		void GenerateDirectoriesIfMissing()
		{
			if (!std::filesystem::exists(s_projectPath / "Assets"))
				std::filesystem::create_directory(s_projectPath / "Assets");
		}

		bool HasProjectFile(std::filesystem::path path)
		{
			return std::filesystem::exists(path / "project.helios");
		}

		void CreateProject(std::filesystem::path path, std::function<void(std::string, float)> progressCb)
		{

			if (HasProjectFile(path))
			{
#ifdef HELIOS_PLATFORM_WINDOWS
				MessageBoxA(NULL, "The directory given is already a project!", "Failed to create project!", MB_ICONERROR);
#endif
				throw HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
			}

			try
			{
				if (!std::filesystem::exists(path))
					std::filesystem::create_directories(path);

			} catch (std::filesystem::filesystem_error e)
			{
#ifdef HELIOS_PLATFORM_WINDOWS
				MessageBoxA(NULL, e.what(), "Failed to create project!", MB_ICONERROR);
#endif
				std::cout << e.what() << std::endl;
				exit(1);
			}

			progressCb("Creating project directory structure...", 0.0f);

			s_projectPath = path;
			s_projectSettings.name = (path.filename().empty() ? path.parent_path() : path).filename().string();
			if (s_projectSettings.name.empty()) s_projectSettings.name = "Untitled";
			
			s_projectSettings.Serialize();

			GenerateDirectoriesIfMissing();
		}

		void LoadProject(std::filesystem::path path, std::function<void(std::string, float)> progressCb)
		{
			if (!HasProjectFile(path))
			{
#ifdef HELIOS_PLATFORM_WINDOWS
				MessageBoxA(NULL, "The directory given is not a project!", "Failed to load project!", MB_ICONERROR);
#endif
				throw HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
			}

			progressCb("Loading project...", 0.0f);

			s_projectPath = path;

			try
			{
				s_projectSettings.Deserialize();
			} catch (YAML::Exception e)
			{
				std::cout << e.what() << std::endl;
#ifdef HELIOS_PLATFORM_WINDOWS
				MessageBoxA(NULL, e.what(), "Failed to load project!", MB_ICONERROR);
#endif
				exit(1);
			}

			progressCb("Loading assets...", 0.5f);
		}

		const std::filesystem::path GetLastScenePath()
		{
			std::string str;
			std::ifstream in(s_projectPath / "Temp");
			in >> str;
			in.close();
			return str;
		}

		void SetLastScenePath(std::filesystem::path scene_path)
		{
			std::ofstream out(s_projectPath / "Temp");
			out << scene_path;
			out.close();
		}

		inline StartupConfig& GetStartupConfig() { return s_startupConfig; }

		// void AttemptLoad() {
		// 	if (__argc > 1) {
		// 		int argCount;
		// 		LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argCount);
		// 		args[1][wcslen(args[1]) - 1] = '\0';
		// 		std::filesystem::path selected_path(args[1]);
		// 		LocalFree(args);

		// 		std::cout << "Project Location: " <<  selected_path << std::endl;

		// 		if (!std::filesystem::is_directory(selected_path)) {
		// 			MessageBoxA(NULL, "The path given is invalid!", "Failed to open project!", MB_ICONERROR);
		// 			throw HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
		// 		}

		// 		int code;
		// 		if ((code = Helios::Project::IsValidProject(selected_path.string().c_str())) < 0) {
		// 			std::cout << "Given directory: \"" << selected_path << "\" is not a valid project! Code: " << code << std::endl;
		// 			if (MessageBoxA(NULL, "The directory given is not a valid project! Do you want to create a new project in this directory", "Failed to open project!", MB_RETRYCANCEL | MB_ICONERROR) != IDRETRY)
		// 				throw HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
		// 			else {
		// 				Project::CreateNewProject(selected_path.string().c_str());
		// 				if ((code = Helios::Project::IsValidProject(selected_path.string().c_str())) < 0)
		// 				{
		// 					MessageBoxA(NULL, "Failed to recreate project!", "Failed to open project!", MB_ICONERROR);
		// 					throw HELIOS_EXIT_CODE_INVALID_PROJECT_PATH;
		// 				}
		// 			}
		// 		}

		// 		s_startupConfig = Project::DeserializeStartupConfig((selected_path / "Settings" / "Startup.config").string().c_str());
		// 		s_projectPath = selected_path;
		// 	}
		// 	else {
		// 		ShellExecuteA(NULL, "open", std::filesystem::current_path().append("ProjectExplorer.exe").string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
		// 		throw HELIOS_EXIT_CODE_INVALID_ARGS;
		// 	}
		// }
		void SaveScene(bool as_new) {
			//if (s_currentScenePath.empty() || as_new)
			//	SaveSceneDialog();
			//else {
			//	//SceneRegistry::GetCurrentScene().Serialize(s_currentScenePath.string(), SceneRegistry::GetCurrentScene());
			//}
			//auto fn = s_currentScenePath.filename().string();
			//SetWindowTextA(DepricatedApplication::GetHwnd(), std::string_view(fn.c_str(), fn.size() - 6).data());
		}
		void SaveSceneDialog() {
			//if (SceneRegistry::get_current_scene()) {
			//	wchar_t file[MAX_PATH];
			//	ZeroMemory(file, sizeof(MAX_PATH));

			//	OPENFILENAMEW props;
			//	ZeroMemory(&props, sizeof(props));
			//	props.lStructSize = sizeof(props);
			//	props.hwndOwner = DepricatedApplication::GetHwnd();
			//	props.lpstrFilter = L"Scene (*.scene)\0*.scene\0All Files (*.*)\0*.*\0";
			//	props.lpstrFile = file;
			//	props.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			//	props.lpstrDefExt = L"scene";
			//	props.nMaxFile = MAX_PATH;
			//	props.lpstrInitialDir = Project::GetAssetsPath().wstring().c_str();

			//	if (GetSaveFileName(&props)) {
			//		std::wstring wstr(file);
			//		std::string str(wstr.begin(), wstr.end());
			//		if (ENDS_WITH(str, ".scene"))
			//		{
			//			s_currentScenePath = str;
			//			//Scene::Serialize(str, SceneRegistry::GetCurrentScene());
			//		}
			//		else
			//			DepricatedApplication::ShowMessage("Failed to save scene!", "File must end with .scene extention!", MB_ICONERROR, true);
			//	}
			//}
		}
		void TryLoad(std::filesystem::path last) {
			if (last != "" || std::filesystem::exists(last)) 
			{
				auto name = s_currentScenePath.filename().string();
				name = name.substr(0, name.length() - 6);
				
				LoadSampleScene();
			}
			else LoadSampleScene();
		}
		void LoadSampleScene() {
			/*auto scene = SceneRegistry::create_temporary_scene();
			scene->CreateMainCamera({});
			scene->Init();*/
		}

		void LoadAssets()
		{
			// Check 
		}

		void OpenSceneDialog() {
		/*	if (SceneRegistry::get_current_scene()) {
				wchar_t file[MAX_PATH];
				ZeroMemory(file, sizeof(MAX_PATH));

				OPENFILENAMEW props;
				ZeroMemory(&props, sizeof(props));
				props.lStructSize = sizeof(props);
				props.hwndOwner = DepricatedApplication::GetHwnd();
				props.lpstrFilter = L"Scene (*.scene)\0*.scene\0All Files (*.*)\0*.*\0";
				props.lpstrFile = file;
				props.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
				props.lpstrDefExt = L"scene";
				props.nMaxFile = MAX_PATH;
				props.lpstrInitialDir = Project::GetAssetsPath().wstring().c_str();

				if (GetSaveFileName(&props)) {
					std::wstring wstr(file);
					std::string str(wstr.begin(), wstr.end());
					if (ENDS_WITH(str, ".scene"))
					{
						s_currentScenePath = str;
						SceneRegistry::Register(s_currentScenePath);
						SceneRegistry::LoadScene(s_currentScenePath);
					}
					else
						DepricatedApplication::ShowMessage("Failed to save scene!", "File must end with .scene extention!", MB_ICONERROR, true);
				}
			}*/	
		}
	}
}