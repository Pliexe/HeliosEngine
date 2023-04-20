-- HeliosEngine - A cross-platform game engine
-- HeliosEditor - A cross-platform game editor using HeliosEngine
-- HeliosEditor dynamically links to HeliosEngine

if _ACTION == 'clean' then
    os.rmdir('bin')
    os.rmdir('bin-int')
    os.remove('**.sln')
    os.remove('**.vcxproj')
    os.remove('**.vcxproj.*')
    -- etc
end 

workspace "HeliosEngine"
    architecture "x64"
    startproject "HeliosEditor"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
	{
		"MultiProcessorCompile"
	}

outdir = "%{cfg.system}/%{cfg.buildcfg}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["ImGui"] = "%{wks.location}/HeliosEngine/vendor/imgui"
IncludeDir["ImGuiMisc"] = "%{wks.location}/HeliosEngine/vendor/imgui/misc/cpp"
IncludeDir["ImGuiBackends"] = "%{wks.location}/HeliosEngine/vendor/imgui/backends"
IncludeDir["Yaml"] = "%{wks.location}/HeliosEngine/vendor/yaml-cpp/include"
IncludeDir["ProjectManager"] = "%{wks.location}/ProjectManager/src"

group "Dependencies"
    include "HeliosEngine/vendor"

    project "ProjectManager"
        location "ProjectManager"
        kind "SharedLib"
        language "C++"
        staticruntime "off"
        cppdialect "C++17"

        targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

        files
        {
            "ProjectManager/src/**.c",
            "ProjectManager/src/**.cpp",
        }

        includedirs
        {
            "ProjectManager/src",
            "%{wks.location}/HeliosEditor/src",
            "%{IncludeDir.Yaml}",
        }

        links
        {
            "Yaml",
        }

        defines
        {
            "YAML_CPP_STATIC_DEFINE",
        }

        flags { "NoPCH" }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outdir .. "/HeliosEditor")
        }

        filter "system:windows"
            systemversion "latest"
            defines { "HELIOS_PLATFORM_WINDOWS" }

        filter "configurations:Debug"
            defines "HELIOS_DEBUG"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines "HELIOS_RELEASE"
            runtime "Release"
            optimize "on"

        
        
group ""

    include "HeliosEngine"
    include "HeliosEditor"