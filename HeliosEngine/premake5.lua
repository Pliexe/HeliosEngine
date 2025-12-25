project "HeliosEngine"
    kind "SharedLib"
    language "C++"
    staticruntime "off"
    cppdialect "C++20"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    postbuildcommands
    {
        -- ("{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outdir .. "/HeliosEditor/"),
        
        -- ("{COPY} %{wks.location}/bin/" .. outdir .. "/HeliosEngine/*" .. " %{cfg.buildtarget.relpath}/"),
    }
    
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.hpp",
        "src/**.cpp",
        "src/**.inl",
        "src/**.ixx",
        "vendor/stb/include/stb_image.h",
    }

    includedirs
    {
        "src",
        "vendor/stb/include",
        "vendor/hostfxr",
        "vendor/xxhash",
        "vendor/entt/single_include/entt",
        "vendor/json/single_include/nlohmann",
        "vendor/libwebp/src",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuiMisc}",
        "%{IncludeDir.ImGuiBackends}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Yaml}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.Box2D}",
        "%{IncludeDir.VulkanSDK}",
        -- "%{IncludeDir.SPIRV-Reflect}",
        "vendor/spirv-reflect",
    }

    links
    {
        "ImGui",
        "Yaml",
        "GLFW",
        "GLAD",
        "Box2D",
        "xxhash",
        "dxcompiler",
        "libwebp",
        "SPIRV-Reflect",
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

    -- postbuildcommands
    -- {
    --     ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outdir .. "/HeliosEditor/"),
    -- }

    filter { "files:**/vendor/**", "toolset:clang" }
        buildoptions { "-Wno-nontrivial-memcall" }

    filter { "files:**/vendor/**" }
        warnings "Off"

    filter "system:windows"
        systemversion "latest"

        links
        {
            "d3d11.lib",
            "d3dcompiler.lib",
            "d2d1.lib",
            "dwrite.lib",
            "Windowscodecs.lib",
            "Dwmapi.lib",
            "opengl32.lib",
            "%{VULKAN_SDK}/Lib/shaderc_shared.lib",
            "%{VULKAN_SDK}/Lib/glslang.lib",
            "%{Library.VulkanUtils}",
        }

        defines
        {
            "HELIOS_PLATFORM_WINDOWS",
            "STBI_WINDOWS_UTF8",
        }

    filter "system:not windows"
        -- Exclude Windows/Direct3D11 files on non-Windows platforms
        removefiles { "src/Platform/Direct3D11/**", "src/Platform/Windows/**" }

    filter "system:linux"
        pic "on"
        systemversion "latest"

        includedirs { 
            "vendor/dxc/include",
            -- "/usr/include/glib-2.0/glib"
            -- "/usr/include/gtk-4.0"

            -- QT
            -- "/usr/include/qt6/",
        }

        -- libdirs { "/usr/lib/qt6" }

        links
        {
            "GL",
            "vulkan",
            "shaderc_shared",
            "glslang",

            -- QT
            -- "Qt6Widgets",
            -- "Qt6Gui",
            -- "Qt6Core",
            
            "wayland-client", "wayland-cursor", "wayland-egl",
        }
        
        -- includedirs
        -- {
        --     "/usr/include/linux",
        --     "/usr/include/gtk-3.0",
        --     "/usr/include/glib-2.0",
        --     "/usr/lib64/glib-2.0/include",
        --     "/usr/include/pango-1.0",
        --     "/usr/include/harfbuzz",
        --     "/usr/include/cairo",
        --     "/usr/include/gdk-pixbuf-2.0",
        --     "/usr/include/atk-1.0"
        -- }

        defines
        {
            "HELIOS_PLATFORM_LINUX",
            "USE_ZENITY",
            "_GLFW_X11",
            "_GLFW_WAYLAND",
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
    
