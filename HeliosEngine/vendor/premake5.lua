project "ImGui"
    kind "StaticLib"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files {
        "imgui/*.cpp",
        "imgui/*.h",
        "imgui/misc/cpp/*.cpp",
        "imgui/misc/cpp/*.h",
    }

    includedirs { "imgui", "imgui/misc/cpp", "imgui/backends" }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "on"
        buildoptions { "/MP" }
        files {
            "imgui/backends/imgui_impl_dx11.cpp",
            "imgui/backends/imgui_impl_dx11.h",
            "imgui/backends/imgui_impl_dx10.cpp",
            "imgui/backends/imgui_impl_dx10.h",
        }

    filter "system:linux"
        pic "on"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "on"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "Yaml"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
	{
		"yaml-cpp/src/**.h",
		"yaml-cpp/src/**.cpp",
		
		"yaml-cpp/include/**.h",
	}

    includedirs { "yaml-cpp/include" }

    defines { "YAML_CPP_STATIC_DEFINE" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "off"
        buildoptions { "/MP" }

    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "off"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"