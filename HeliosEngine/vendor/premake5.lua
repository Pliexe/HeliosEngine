project "ImGui"
    kind "StaticLib"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    links
    {
        "GLFW",
    }
    
    files {
        "imgui/*.cpp",
        "imgui/*.h",
        "imgui/misc/cpp/*.cpp",
        "imgui/misc/cpp/*.h",
        -- opengl3
        "imgui/backends/imgui_impl_opengl3.cpp",
        "imgui/backends/imgui_impl_opengl3.h",
        "imgui/backends/imgui_impl_glfw.cpp",
        "imgui/backends/imgui_impl_glfw.h",
        "imgui/backends/imgui_impl_opengl3_loader.h",
    }

    includedirs { 
        "imgui", "imgui/misc/cpp", "imgui/backends",
        "glfw/include",
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "on"
        files {
            "imgui/backends/imgui_impl_dx11.cpp",
            "imgui/backends/imgui_impl_dx11.h",
            "imgui/backends/imgui_impl_dx10.cpp",
            "imgui/backends/imgui_impl_dx10.h",

            "imgui/backends/imgui_impl_win32.cpp",
            "imgui/backends/imgui_impl_win32.h",
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
    
    project "GLFW"
        kind "StaticLib"
        language "C"

        targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

        files
        {
            "glfw/include/GLFW/glfw3.h",
            "glfw/include/GLFW/glfw3native.h",
            "glfw/src/glfw_config.h",
            "glfw/src/context.c",
            "glfw/src/init.c",
            "glfw/src/input.c",
            "glfw/src/monitor.c",
            "glfw/src/vulkan.c",
            "glfw/src/window.c",
        }

    filter "system:windows"
        systemversion "latest"
        staticruntime "on"
        
        files
        {
            "glfw/src/win32_init.c",
            "glfw/src/win32_joystick.c",
            "glfw/src/win32_monitor.c",
            "glfw/src/win32_time.c",
            "glfw/src/win32_thread.c",
            "glfw/src/win32_window.c",
            "glfw/src/wgl_context.c",
            "glfw/src/egl_context.c",
            "glfw/src/osmesa_context.c",
        }

        defines 
        { 
            "_GLFW_WIN32"
        }

    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "on"

        defines 
        { 
            "_GLFW_X11"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "GLAD"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
    {
        "glad/include/glad/glad.h",
        "glad/include/KHR/khrplatform.h",
        "glad/src/glad.c",
    }

    includedirs { "glad/include" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "on"

    filter "system:linux"
        pic "on"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
project "Box2D"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "off"

    -- kind "SharedLib"
    -- language "C++"
    -- staticruntime "off"
    -- cppdialect "C++11"

    -- removeflags "MultiProcessorCompile"
    
    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
    {
        "box2d/src/**.cpp",
        "box2d/src/**.h",
    }

    -- postbuildcommands
    -- {
    --     ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outdir .. "/HeliosEditor"),
    -- }

    includedirs
    { 
        "box2d/include"
    }

    filter "system:windows"
        systemversion "latest"
    
    filter "system:linux"
        pic "on"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"