project "SPIRV-Reflect"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
    {
        "spirv-reflect/spirv_reflect.c",
        "spirv-reflect/spirv_reflect.h"
    }

    includedirs { "spirv-reflect" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "on"

    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "on"

    filter "system:macosx"
        pic "on"
        systemversion "latest"
        staticruntime "on"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "libwebp"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
    {
        "libwebp/src/**.h",
        "libwebp/src/**.c",
    }

    includedirs { "libwebp" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "on"

    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "on"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

project "xxhash"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

    files
    {
        "xxhash/xxhash.c",
        "xxhash/xxhash.h",
    }

    includedirs { "xxhash" }

    filter "system:windows"
        systemversion "latest"
        staticruntime "on"

    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "on"

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

    includedirs { "%{IncludeDir.VulkanSDK}" }
    links { "vulkan" }

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

        -- NEW! GLFW 3.4+
        "glfw/src/platform.c",
        "glfw/src/null_init.c",
        "glfw/src/null_joystick.c",
        "glfw/src/null_monitor.c",
        "glfw/src/null_window.c",
        "glfw/src/init.c",
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
            "_GLFW_WIN32",
        }

    filter "system:linux"
        pic "on"
        systemversion "latest"
        staticruntime "on"

        local binint = "%{wks.location}/bin-int/" .. outdir .. "/GLFW/generated"
        local wayland_deps = path.getabsolute("glfw/deps/wayland")

        prebuildcommands { "mkdir -p " .. binint }

        function getXmlFiles(dir)
            local files = {}

            local function scan(d)
                for _, f in ipairs(os.matchfiles(d .. "/*.xml")) do
                    print(f)
                    table.insert(files, f)
                end
                for _, sub in ipairs(os.matchdirs(d .. "/*")) do
                    print(sub)
                    scan(sub)
                end
            end

            scan(dir)
            return files
        end

        local xmls = getXmlFiles(wayland_deps)

        for _, xml in ipairs(xmls) do
            local name = path.getbasename(xml)

            local client_header = path.join(binint, name .. "-client-protocol.h")
            local private_code  = path.join(binint, name .. "-client-protocol-code.h")

            print("wayland-scanner client-header " .. xml .. " " .. client_header)
            prebuildcommands {
                "wayland-scanner client-header " .. xml .. " " .. client_header,
                "wayland-scanner private-code  " .. xml .. " " .. private_code
            }
        end

        includedirs { binint }


        links { "wayland-client", "wayland-cursor", "wayland-egl" }

        files
        {
            -- X11
            "glfw/src/x11_init.c",
            "glfw/src/x11_monitor.c",
            "glfw/src/x11_window.c",
            "glfw/src/xkb_unicode.c",

            -- Wayland
            "glfw/src/wl_init.c",
            "glfw/src/wl_monitor.c",
            "glfw/src/wl_window.c",
            "glfw/src/wl_platform.h",

            -- Common POSIX
            "glfw/src/posix_time.c",
            "glfw/src/posix_poll.c",
            "glfw/src/posix_module.c",
            "glfw/src/posix_thread.c",
            "glfw/src/linux_joystick.c",
            "glfw/src/egl_context.c",
            "glfw/src/osmesa_context.c",
            "glfw/src/glx_context.c",
        }

        defines 
        { 
            "_GLFW_X11",
            "_GLFW_WAYLAND",
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

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
        "imgui/backends/imgui_impl_vulkan.cpp",
        "imgui/backends/imgui_impl_vulkan.h",
    }

    includedirs { 
        "glfw/include",
        "imgui", "imgui/misc/cpp", "imgui/backends",
        "%{IncludeDir.VulkanSDK}",
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