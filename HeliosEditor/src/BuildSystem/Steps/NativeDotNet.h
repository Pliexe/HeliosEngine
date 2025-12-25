#pragma once
#include "../BuildTask.h"
#include "../RunCommand.h"
#include "ProjectManager.h"

namespace Helios
{
    template <bool DebugMode>
    inline static BuildStep::Action EditorDotNetBuildAction = [](BuildStep::ProgressCallback callback) { 
        callback("Starting Build", 0.0f);
        // bool DebugMode = true;
		std::filesystem::path usr_csproj_file = Project::GetProjectPath() / "BuildSystems" / "DotNet" / "UserScript" / "UserScript.csproj";
        auto result = RunCommand(u8"dotnet publish \"" + usr_csproj_file.u8string() + u8"\" -r win-x64" + (DebugMode ? u8" -c Debug" : u8" -c Release"), [](const std::string& command) 
        { 
            std::wcout << L"Build command: " << conversions::from_utf8_to_utf16(command) << L"\n"; 
        });
        
        if (result.exitCode != 0)
        {
            callback("Build Failed", 1.0f);
            return false;
        }

        std::cout << "Finished Build" << std::endl << std::endl;

        std::cout << result.output << std::endl << std::endl;

        callback("Finished Build", 1.0f);
        return true; 
    };
}