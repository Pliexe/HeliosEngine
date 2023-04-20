project "HeliosEditor"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.hpp",
        "src/**.cpp",
    }

    includedirs
    {
        "src",

        "%{wks.location}/HeliosEngine/src",
        "%{wks.location}/HeliosEngine/vendor/entt/include",
        "%{wks.location}/HeliosEngine/vendor/json/single_include/nlohmann",
        
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuiMisc}",
        "%{IncludeDir.ImGuiBackends}",
        "%{IncludeDir.Yaml}",
        "%{IncludeDir.ProjectManager}",
    }

    links
    {
        "HeliosEngine",
        "ImGui",
        "Yaml",
        "ProjectManager",
    }

    defines
    {
        "HELIOS_EDITOR",
        "HELIOS_INCLUDE_IMGUI",
        "YAML_CPP_STATIC_DEFINE",
    }

    -- For some reason it's not settings it inside Visual Studio
    debugargs { "\"C:\\Users\\dz\\Documents\\Helios Projects\\Sandbox\\\"" }

    flags { "NoPCH" }

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