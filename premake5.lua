-- HeliosEngine - A cross-platform game engine
-- HeliosEditor - A cross-platform game editor using HeliosEngine
-- HeliosEditor dynamically links to HeliosEngine

-- Build files for other editors
require "vendor/premake-vscode/vscode"
require "vendor/premake-cmake/cmake"

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

    defines
    {
        "HELIOS_IMGUI_ENABLED",
        "HL_PROFILE",

        -- Windows
        "NOMINMAX",
    }


local function getDotNetSdkPath()
    local dotnet = os.getenv("DOTNET_EXE") or "dotnet"
    local pipe = io.popen('"' .. dotnet .. '" --list-sdks')
    local sdks = pipe:read('*a')
    pipe:close()

    local sdk = sdks:match('([^\n]+)')

    if not sdk then
        error("Failed to find .NET SDK")
    end

    return sdk
end

VULKAN_SDK = os.getenv("VULKAN_SDK")
    
local DOTNETSDK_DIR = getDotNetSdkPath()

if DOTNETSDK_DIR then
    print("Found .NET SDK at " .. DOTNETSDK_DIR)
end

outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["ImGui"] = "%{wks.location}/HeliosEngine/vendor/imgui"
IncludeDir["ImGuiMisc"] = "%{wks.location}/HeliosEngine/vendor/imgui/misc/cpp"
IncludeDir["ImGuiBackends"] = "%{wks.location}/HeliosEngine/vendor/imgui/backends"
IncludeDir["Yaml"] = "%{wks.location}/HeliosEngine/vendor/yaml-cpp/include"
IncludeDir["ProjectManager"] = "%{wks.location}/ProjectManager/src"
IncludeDir["GLFW"] = "%{wks.location}/HeliosEngine/vendor/glfw/include"
IncludeDir["GLAD"] = "%{wks.location}/HeliosEngine/vendor/glad/include"
IncludeDir["Box2D"] = "%{wks.location}/HeliosEngine/vendor/box2d/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

Library = {}

Library["VulkanUtils"] = "%{VULKAN_SDK}/Lib/Vulkan-1.lib"

Library["ShaderC_Debug"] = "%{VULKAN_SDK}/Lib/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{VULKAN_SDK}/Lib/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{VULKAN_SDK}/Lib/spirv-cross-glsld.lib"
Library["SPIRV_Cross_HLSL_Debug"] = "%{VULKAN_SDK}/Lib/spirv-cross-hlsld.lib"

Library["ShaderC_Release"] = "%{VULKAN_SDK}/Lib/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{VULKAN_SDK}/Lib/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{VULKAN_SDK}/Lib/spirv-cross-glsl.lib"
Library["SPIRV_Cross_HLSL_Release"] = "%{VULKAN_SDK}/Lib/spirv-cross-hlsl.lib"

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
            defines { "HELIOS_PLATFORM_WINDOWS", "HELIOS_DEBUG" }

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

newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate files, and project files",
    execute = function ()
        print("Removing all binaries and intermediate files")
        os.rmdir('bin')
        os.rmdir('bin-int')
        print("Removing all project files")
        os.remove('**.sln')
        os.remove('**.vcxproj')
        os.remove('**.vcxproj.*')
        -- codelite
        os.remove('**.workspace')
        os.remove('**.userprefs')
        os.remove('**.project')
        os.remove('**.mk')
        os.remove('.codelite') -- Needs to fix it
        -- vscode
        os.remove('**.code-workspace')
        print("Removing Makefile and Makefile.*")
        os.remove('**Makefile')
        os.remove('**Makefile.*')
        os.remove('**.make')
        print("Done")
    end
}
