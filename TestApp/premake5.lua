project "TestApp"
    kind "ConsoleApp"
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
        "Shaders/**.hlsl",
    }

    includedirs
    {
        "src",

        "%{wks.location}/HeliosEngine/src",
        "%{wks.location}/HeliosEngine/vendor/entt/single_include/entt",
        "%{wks.location}/HeliosEngine/vendor/json/single_include/nlohmann",

        "vendor/filewatch",
        
        "%{IncludeDir.ProjectManager}",
        
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuiMisc}",
        "%{IncludeDir.Yaml}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
    }

    links
    {
        "HeliosEngine",
        "ImGui",
        "Yaml",
        "ProjectManager",
        "GLFW",
        "GLAD",
        "Box2D",
        (os.target() == "linux" or os.target() == "macos") and "dxcompiler" or nil,
    }

    defines
    {
        "HELIOS_EDITOR",
        "HELIOS_BUILD_DLL",
        "HELIOS_INCLUDE_IMGUI",
        "YAML_CPP_STATIC_DEFINE",
        "GLFW_INCLUDE_NONE",
    }

    -- For some reason it's not settings it inside Visual Studio
    debugargs { "-project \"C:\\Users\\dz\\Documents\\Helios Projects\\Sandbox\"" }

    flags { "NoPCH" }

    -- filter { "files:**.hlsl" }
    --     buildcommands 'dxc.exe -spirv -T vs_5_0 -E main .\\%{file.basename}.hlsl -Fo .\\%{file.basename}.spv'

    -- Assuming your shaders are located in ../data/shaders/
    filter "files:../data/shaders/*.hlsl"
        buildcommands '"../libs/vulkan/glslangValidator.exe" -V -o "%{file.directory}/bin/%{file.name}.spv" "%{file.relpath}"'
        buildoutputs "%{file.directory}/bin/%{file.name}.spv"


    filter { "files:**.hlsl" }
        flags "ExcludeFromBuild"
        -- shadermodel "5.0"
        shadermodel "6.0"
        shaderobjectfileoutput("CompiledShaders/%{file.basename}"..".cso")
        shaderassembleroutput("CompiledShaders/%{file.basename}"..".asm")
        
    filter { "files:**PixelShader.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Pixel"
        shaderentry "main"
    filter { "files:**VertexShader.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Vertex"
        shaderentry "main"
    filter { "files:**GeometryShader.hlsl" }
        removeflags "ExcludeFromBuild"
        shadertype "Geometry"
        shaderentry "main"

    filter "system:windows"
        systemversion "latest"
        entrypoint "mainCRTStartup"
        defines { "HELIOS_PLATFORM_WINDOWS" }
        links
        {
            "Gdiplus.lib",
            "opengl32.lib",
            "%{Library.VulkanUtils}",
            "%{VULKAN_SDK}/Lib/shaderc_shared.lib",
        }
        postbuildcommands
        {
            "{COPY} %{wks.location}/bin/" .. outdir .. "/HeliosEngine/*.dll %{cfg.buildtarget.directory}/",
        }

    filter "system:linux"
        pic "on"
        systemversion "latest"
        defines { "HELIOS_PLATFORM_LINUX" }

        libdirs { "/usr/lib/qt6" }

        links
        {
            -- "dl",
            "GL",
            "vulkan",
            "shaderc_shared",
            "glslang",
            -- "pthread",
            -- "%{Library.VulkanUtils}",

            
            -- QT
            "Qt6Widgets",
            "Qt6Gui",
            "Qt6Core",
            "X11", "cairo"
        }


    filter "configurations:Debug"
        defines "HELIOS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"

        defines "HELIOS_RELEASE"
        runtime "Release"
        optimize "on"