project "HeliosEngine"
    kind "SharedLib"
    language "C++"
    staticruntime "off"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"
    
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.hpp",
        "src/**.cpp",
        "src/**.inl",
        "vendor/stb/include/stb_image.h",
    }

    includedirs
    {
        "src",
        "vendor/stb/include",
        "vendor/entt/include",
        "vendor/json/single_include/nlohmann",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuiMisc}",
        "%{IncludeDir.ImGuiBackends}",
        "%{IncludeDir.Yaml}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.Box2D}",
        "%{IncludeDir.VulkanSDK}",
    }

    links
    {
        "ImGui",
        "Yaml",
        "GLFW",
        "GLAD",
        "Box2D",
        "opengl32.lib",
        "%{Library.VulkanUtils}",
        "%{VULKAN_SDK}/Lib/shaderc_shared.lib",
        "%{VULKAN_SDK}/Lib/glslang.lib",
    }

    defines
    {
        "HELIOS_BUILD_DLL",
        "HELIOS_EXPORT_DLL",
        "HELIOS_EDITOR",
        "HELIOS_INCLUDE_IMGUI",
        "YAML_CPP_STATIC_DEFINE",
        "GLFW_INCLUDE_NONE",
        "GLFW_INCLUDE_VULKAN",
    }

    flags { "NoPCH" }

    postbuildcommands
    {
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outdir .. "/HeliosEditor"),
    }

    filter "system:windows"
        systemversion "latest"

        links
        {
            "d3d11.lib",
            "d3dcompiler.lib",
            "d2d1.lib",
            "dwrite.lib",
            "Windowscodecs.lib",
        }

        defines
        {
            "HELIOS_PLATFORM_WINDOWS",
            "STBI_WINDOWS_UTF8",
        }

    filter "system:linux"
        pic "on"
        systemversion "latest"
        
        includedirs
        {
            "/usr/include/linux",
            "/usr/include/gtk-3.0",
            "/usr/include/glib-2.0",
            "/usr/lib64/glib-2.0/include",
            "/usr/include/pango-1.0",
            "/usr/include/harfbuzz",
            "/usr/include/cairo",
            "/usr/include/gdk-pixbuf-2.0",
            "/usr/include/atk-1.0"
        }

        defines
        {
            "HELIOS_PLATFORM_LINUX",
        }

    filter "configurations:Debug"
        defines "HELIOS_DEBUG"
        runtime "Debug"
        symbols "on"

        links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}",
			"%{Library.SPIRV_Cross_HLSL_Debug}",
		}

    filter "configurations:Release"
        defines "HELIOS_RELEASE"
        runtime "Release"
        optimize "on"

        links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}",
            "%{Library.SPIRV_Cross_HLSL_Release}",
		}
    
