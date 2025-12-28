#include "Application.h"
#include "Helios/Core/EntryPoint.h"
#include <Helios/Core/Application.h>


#include <iostream>

#include <Helios.h>
#include <Helios/Resources/Color.h>

#include <Helios/Utils/ErrorHandling.h>

#include "Helios/Graphics/CommandAllocator.h"
#include "Helios/Resources/ResourceRegistry.h"
#include "Helios/Utils/ErrorHandling.h"
#include <filesystem>

#include "Panels/InspectorPanel.h"
#include "ProjectManager.h"
#include <Panels/ProjectExplorer.h>


#include "Icons.h"

#include "HeliosEditor_Macros.h"

// #include <Shlwapi.h> what is this lol?

#include "Helios/Math/Matrix.h"
#include <Helios/Math/Quaternion.h>
#include <sstream>

#include <Helios/Scripting/ServiceResult.h>

#include "AssetRegistry.h"
#include <Helios/Input/InputManager.h>
#include <Helios/Input/KeyCodes.h>

#include "ApplicationStyle.h"
#include "Helios/Core/Profiler.h"

#include "Helios/Scene/EditorCamera.h"

#include "GUI/Toolbar.h"
#include "Helios/Core/UUID.h"
#include "Helios/Resources/Material.h"
#include "InitWindow.h"
#include "Panels/ExplorerPanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/ScriptingServices.h"
#include "Panels/ShadowMaps.h"


#include <FileWatch.hpp>

/// <summary>
/// RUST SCRIPT MANAGER BRUH REMOVE LATER THIS IS TMP
/// </summary>
#include <Helios/Scripting/Rust/RustScriptsManager.h>

#include "Helios/Core/AssetManager.h"
#include "Helios/Debug/CacheDebugger.h"

static std::filesystem::path currentScene;
StartupConfig startupConfig;

using namespace Helios;

bool isGameSceneActive = false;
bool isEditorSceneActive = false;

struct TransformVertex
{
    Vector3 position;
    Color color;
};

int ValidateInit()
{

    try
    {
        /*Project::AttemptLoad();
        SetCurrentPath(Project::GetAssetsPath());*/
    }
    catch (int code)
    {
        return code;
    }

    return 0;
}

#include "imgui.h"

bool generate_proj = false;
inline std::filesystem::path PROJECT_PATH;

Helios::Application* Helios::CreateApplication(int argc, char** argv)
{
    Application::Specifications specs;

#ifdef HELIOS_PLATFORM_WINDOWS
    specs.graphicsAPI = Graphics::API::Direct3D11;
#else
    specs.graphicsAPI = Graphics::API::Vulkan;
#endif
    // specs.graphicsAPI = Graphics::API::Vulkan;
    specs.width = 1280;
    specs.height = 720;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            std::cout << "Helios Editor" << std::endl;
            std::cout << "Usage: HeliosEditor.exe [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "\t-h - Show this help message" << std::endl;
            std::cout << "\t-project <path> - Open project at <path>"
                      << std::endl;
            exit(0); // NOLINT(concurrency-mt-unsafe)
        }

        if (strcmp(argv[i], "--project") == 0 && (i + 1 < argc))
        {
            PROJECT_PATH = argv[i + 1];
#ifdef HELIOS_PLATFORM_LINUX
            if (!PROJECT_PATH.u8string().empty() &&
                PROJECT_PATH.u8string().starts_with(u8"~/"))
            {
                const char* home_dir = std::getenv("HOME");
                if (home_dir)
                    PROJECT_PATH = std::filesystem::path(home_dir) /
                                   PROJECT_PATH.u8string().substr(2);
            }
#endif

            if (std::filesystem::exists(PROJECT_PATH) &&
                PROJECT_PATH.has_extension() &&
                PROJECT_PATH.extension() == ".helios")
            {
                PROJECT_PATH = PROJECT_PATH.parent_path();
            }
        }

        if (strcmp(argv[i], "--generate") == 0)
            generate_proj = true;
    }

    specs.title = "Helios Editor";

    switch (specs.graphicsAPI)
    {
    case Graphics::API::Vulkan:
        specs.title += " (Vulkan)";
        break;
    case Graphics::API::OpenGL:
        specs.title += " (OpenGL)";
        break;
    case Graphics::API::Direct3D11:
        specs.title += " (Direct3D11)";
        break;
    case Graphics::API::None:
        specs.title += " (No Graphics)";
        break;
    case Graphics::API::Direct3D12:
        specs.title += " (Direct3D12)";
        break;
    case Graphics::API::WebGL:
        specs.title += " (WebGL)";
        break;
    case Graphics::API::WebGPU:
        specs.title += " (WebGPU)";
        break;
    }

    specs.style = WindowStyles::Resizable | WindowStyles::Centered |
                  WindowStyles::Decorated | WindowStyles::NoTitlebar;

    return new Helios::HeliosEditor(specs);
}

namespace Helios
{

HeliosEditor::HeliosEditor(const Specifications& specs) : Application(specs)
{

    std::cout << "Initializing HeliosEditor..." << std::endl;
    m_applicationName = "Helios Editor";
    /*GraphicalWindow::Specifications windowSpecs;
            windowSpecs.title = "Loading...";
            windowSpecs.width = 1280;
            windowSpecs.height = 720;
            windowSpecs.style = WindowStyles::Show;
            Win32GraphicalWindow window;
            window.Create(windowSpecs);
            window.GetContext().UseContext();
            window.GetContext().SwapBuffers();
            window.GetContext().SwapBuffers();
            Sleep(5000);
            window.Destroy();*/

    if (std::filesystem::exists(PROJECT_PATH))
    {
        if (Project::HasProjectFile(PROJECT_PATH))
        {
            Project::LoadProject(PROJECT_PATH,
                                 [](std::string cj, float prog) {});
        }
        else if (generate_proj)
        {
            Project::CreateProject(PROJECT_PATH,
                                   [](std::string cj, float prog) {});
        }
        else
        {
            std::cout << "The path given does not contain a Helios project. "
                         "Use --generate if you want to make a new project!"
                      << std::endl;
#ifdef HELIOS_PLATFORM_WINDOWS
            MessageBoxA(nullptr,
                        "The path given does not contain a Helios project.",
                        "Helios Editor", MB_OK | MB_ICONERROR);
#endif
            exit(1); // NOLINT(concurrency-mt-unsafe)
        }
    }
    else
    {
        if (generate_proj)
        {
            Project::CreateProject(PROJECT_PATH,
                                   [](std::string cj, float prog) {});
        }
        else
        {
            std::cout << "The path given does not contain a Helios project. "
                         "Use -generate if you want to make a new project!"
                      << std::endl;
#ifdef HELIOS_PLATFORM_WINDOWS
            MessageBoxA(nullptr,
                        "The path given does not contain a Helios project.",
                        "Helios Editor", MB_OK | MB_ICONERROR);
#endif
            exit(1); // NOLINT(concurrency-mt-unsafe)
        }
    }

    ResourceRegistry::SetResourcePath(Project::GetProjectPath());

    GetWindow()->SetTitle(GetWindow()->GetTitle() +
                          " | Project: " + Project::GetProjectSettings().name);

    ImGui::SetCurrentContext(GetWindow()->CreateImGuiContext());
    GetWindow()->ImpImGui();

    InitImGuiStyle();

    // GetWindow()->GetContext().UseContext();
    GetWindow()->SetVSync(true);

    InitWindow* initWindow = InitWindow::Create();
    initWindow->SetTextA("Loading Engine...");

    try
    {
        constexpr static int to_load_count = 25;

        initWindow->SetB("Loading icon. 1 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FOLDER_EMPTY =
            Texture2D::Create("Resources/Icons/folder_empty.png");
        initWindow->SetB("Loading icon. 2 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_UNKNOWN =
            Texture2D::Create("Resources/Icons/unknown_file.png");
        initWindow->SetB("Loading icon. 3 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_TXT = Texture2D::Create("Resources/Icons/text_file.png");
        initWindow->SetB("Loading icon. 4 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_C = Texture2D::Create("Resources/Icons/c_file.png");
        initWindow->SetB("Loading icon. 5 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_H = Texture2D::Create("Resources/Icons/h_file.png");
        initWindow->SetB("Loading icon. 6 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_CPP = Texture2D::Create("Resources/Icons/cpp_file.png");
        initWindow->SetB("Loading icon. 7 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_HPP = Texture2D::Create("Resources/Icons/hpp_file.png");
        initWindow->SetB("Loading icon. 8 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_RUST = Texture2D::Create("Resources/Icons/rust_file.png");
        initWindow->SetB("Loading icon. 9 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_CSHARP = Texture2D::Create("Resources/Icons/cs_file.png");
        initWindow->SetB("Loading icon. 10 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_FONT = Texture2D::Create("Resources/Icons/ttf_file.png");
        initWindow->SetB("Loading icon. 11 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_SCENE = Texture2D::Create("Resources/Icons/scene_file.png");
        initWindow->SetB("Loading icon. 12 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_IMAGE = Texture2D::Create("Resources/Icons/image_file.png");
        initWindow->SetB("Loading icon. 13 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FILE_OBJECT = Texture2D::Create("Resources/Icons/object_file.png");
        initWindow->SetB("Loading icon. 14 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_PLAY_WHITE = Texture2D::Create("Resources/Icons/play_white.png");
        initWindow->SetB("Loading icon. 15 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_PAUSE_WHITE = Texture2D::Create("Resources/Icons/pause_white.png");
        initWindow->SetB("Loading icon. 16 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_STOP_WHITE = Texture2D::Create("Resources/Icons/stop_white.png");
        initWindow->SetB("Loading icon. 17 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_FOLDER = Texture2D::Create("Resources/Icons/folder.png");

        initWindow->SetB("Loading icon. 18 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_GROUP_TILES_WHITE =
            Texture2D::Create("Resources/ui_explorer_group_tiles_white.png");
        initWindow->SetB("Loading icon. 19 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_GROUP_LIST_WHITE =
            Texture2D::Create("Resources/ui_explorer_group_list_white.png");
        initWindow->SetB("Loading icon. 20 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_GROUP_ICONS_WHITE =
            Texture2D::Create("Resources/ui_explorer_group_icons_white.png");

        initWindow->SetB("Loading icon. 21 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_GROUP_TILES_BLACK =
            Texture2D::Create("Resources/ui_explorer_group_tiles_black.png");
        initWindow->SetB("Loading icon. 22 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_GROUP_LIST_WHITE =
            Texture2D::Create("Resources/ui_explorer_group_list_black.png");
        initWindow->SetB("Loading icon. 23 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_GROUP_ICONS_WHITE =
            Texture2D::Create("Resources/ui_explorer_group_icons_black.png");

        initWindow->SetB("Loading icon. 24 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_REFRESH_WHITE =
            Texture2D::Create("Resources/ui_refresh_white.png");
        initWindow->SetB("Loading icon. 25 out of " +
                             std::to_string(to_load_count),
                         to_load_count);
        ICON_UI_REFRESH_BLACK =
            Texture2D::Create("Resources/ui_refresh_black.png");
    }
    catch (HeliosException e)
    {
        Helios::ShowMessage("Error loading icons!", e.what(),
                            Helios::Message::IconError |
                                Helios::Message::Flags::Ok);
        exit(-100);
    }

    // std::filesystem::path dll_loc = Project::GetProjectPath() /
    // "BuildSystems" / "DotNet" / "UserScript" / "bin" / "Debug" / "net9.0" /
    // "win-x64" / "native" / "UserScript.dll";

    // initWindow->SetA("Building scripts...", 0.0f);
    // initWindow->SetB("Building Native AOT DotNet...", 0.5f);

    // if (!std::filesystem::exists(dll_loc)) m_buildDotNetScriptsTask.Run();

    // initWindow->SetProgressA(1.0f);
    // initWindow->SetProgressB(1.0f);

    Project::LoadAssets();

    if (ICON_FOLDER_EMPTY == nullptr)
        Helios::ShowMessage(
            "Error loading icons!", "ICON_FOLDER_EMPTY == nullptr",
            Helios::Message::IconError | Helios::Message::Flags::Ok);

    // gameFrame = Framebuffer::Create(300, 300,
    // {Framebuffer::Format::R8G8B8A8_UNORM,
    // 										   //
    // Framebuffer::Format::R32F, 										   Framebuffer::Format::D32F});

    initWindow->Quit();

    AssetRegistry::Init();

    using namespace Helios;

    Project::TryLoad();

    // tmp

    SceneRegistry::LoadEmptyScene("New Scene");

    auto material = MaterialInstance::Create(MaterialInstance::Filter::MinMagMipPoint, MaterialInstance::Type::Clamp);
    
    // for (int i = 0; i < 500000; ++i)
    // {
    //     auto ent = SceneRegistry::GetActiveScenes()[0]->CreateEntity("Cube " + std::to_string(i), Vector3{0, 0, 0});
    //     auto& mrc = ent.AddScopedComponent<MeshRendererComponent>();
    //     mrc.mesh = ResourceRegistry::GetResource<Mesh>(MeshType::Cube);
    //     mrc.material = material;
    // }
    auto ent2 = SceneRegistry::GetActiveScenes()[0]->CreateEntity("Directional Light", Vector3{0, 0, 0});
    auto comps = ent2.HasComponent<UUIDComponent>();
    auto comps2 = ent2.HasComponent<TransformComponent>();
    ent2.AddScopedComponent<DirectionalLightComponent>(Color::White, 1.0f);
    Transform(ent2).SetLocalRotation(Quaternion::FromEuler(-45, 0, 0));

    panels.push_back(&inspector);
    panels.push_back(&hierarchy);
    panels.push_back(&gameViewPanel);
    panels.push_back(&scenePanel);
    panels.push_back(new ProfilerPanel());
    panels.push_back(new Editor::ExplorerPanel());
    panels.push_back(new ShadowMapsPanel());
    panels.push_back(new ScriptingServicesPanel());
    // panels.push_back(&AssetRegistry::Get());

    UUID uuid = UUID();
    UUID uuid2 = UUID();
    UUID uuid3 = UUID(uuid);

    std::cout << uuid.toString() << std::endl;
    std::cout << uuid2.toString() << std::endl;
    std::cout << uuid3.toString() << std::endl;

    std::cout << uuid.toString() << " == " << uuid2.toString() << " = "
              << (uuid == uuid2) << std::endl;
    std::cout << uuid.toString() << " == " << uuid3.toString() << " = "
              << (uuid == uuid3) << std::endl;

    uint8_t data[16];
    std::memcpy(data, (uint8_t*)uuid, 16);

    // print whole hex
    std::cout << std::hex;

    for (int i = 0; i < 16; i++)
    {
        std::cout << (int)data[i] << " ";
    }

    UUID uuid4 = UUID::fromString(uuid.toString());
    std::cout << std::endl << uuid4.toString() << std::endl;
    std::cout << uuid4.toString() << " == " << uuid.toString() << " = "
              << (uuid4 == uuid) << std::endl;

    std::cout << std::endl;

    GetWindow()->Show();

    std::string tmp =
        conversions::from_u8string(Project::GetAssetsPath().u8string());

    // Scripting::RegisterService("CSharp", { .shared_library_path =
    // dll_loc.u8string().c_str() });

    // if (auto res = ::Scripting::StartService("CSharp"); res !=
    // ::Scripting::ServiceResult::Ok)
    // {
    // 	std::cout << "Failed to start C# service: " << to_string(res) <<
    // std::endl;
    // }

    watch = CreateUnique<filewatch::FileWatch<std::string>>(
        tmp, std::regex(".*.cs"),
        [](const std::string& path, const filewatch::Event change_type)
        {
            static std::mutex _mutex;
            static bool active = false;
            if (active)
                return;
            active = true;
            std::lock_guard<std::mutex> lock(_mutex);

            std::thread(
                []
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    {
                        std::lock_guard<std::mutex> lock(_mutex);
                        active = false;
                    }
                    reinterpret_cast<HeliosEditor&>(HeliosEditor::GetInstance())
                        .RebuildNextFrame();
                })
                .detach();
        });
}

bool HeliosEditor::BuildDotNet()
{
    std::filesystem::path usr_csproj_file = Project::GetProjectPath() /
                                            "BuildSystems" / "DotNet" /
                                            "UserScript" / "UserScript.csproj";
    std::filesystem::path native_bridge_csproj_file =
        Project::GetProjectPath() / "BuildSystems" / "DotNet" / "NativeBridge" /
        "NativeBridge.csproj";
    // std::filesystem::path bindings_file = Project::GetProjectPath() /
    // "BuildSystems" / "DotNet" / "src" / "bindings.cs";

    // std::ofstream file(bindings_file, std::ios::out);
    // if (!file)
    //{
    //	std::cout << "Unable to open file: \"" << bindings_file << "\"\n";
    //	return;
    // }

    // file.close();

    auto run_command = [](const std::u8string& command)
    {
#ifdef HELIOS_PLATFORM_WINDOWS
        std::wstring wcommand = conversions::from_utf8_to_utf16(command);
        std::cout << "Build command: " << conversions::from_u8string(command)
                  << "\n";

        STARTUPINFO si = {sizeof(STARTUPINFO)};
        PROCESS_INFORMATION pi;

        if (!CreateProcessW(nullptr, &wcommand[0], nullptr, nullptr, FALSE, 0,
                            nullptr, nullptr, &si, &pi))
        {
            std::cerr << "Failed to start process. Error: "
                      << GetLastErrorAsString(GetLastError()) << std::endl;
            Helios::ShowMessage("Failed to run MSBuild",
                                "Error: " +
                                    GetLastErrorAsString(GetLastError()),
                                Message::IconError);
            return;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
#else
        std::string tcommand(conversions::from_u8string(command));
        std::cout << "Build command: " << conversions::from_u8string(command)
                  << "\n";

        int result = std::system(tcommand.c_str());
        if (result != 0)
        {
            std::cerr << "Failed to execute msbuild. Error code: " << result
                      << std::endl;
            return;
        }
#endif
    };

    run_command(u8"dotnet publish \"" + usr_csproj_file.u8string() +
                u8"\" -r win-x64 -c Debug");

    // std::filesystem::path usr_dll_file = Project::GetProjectPath() /
    // "BuildSystems" / "DotNet" / "UserScript" / "bin" / "Debug" / "net9.0" /
    // "win-x64" / "UserScript.dll";

    // if (!std::filesystem::exists(usr_dll_file) ||
    // !std::filesystem::is_regular_file(usr_dll_file)) return false;

    // std::filesystem::path dest = Project::GetProjectPath() / "BuildSystems" /
    // "DotNet" / "NativeBridge" / "UserScript.dll";
    // std::filesystem::copy_file(usr_dll_file, dest,
    // std::filesystem::copy_options::overwrite_existing);

    // run_command(u8"dotnet publish \"" + native_bridge_csproj_file.u8string()
    // + u8"\" -r win-x64 -c Debug");
    // std::filesystem::copy_file()

    return true;
}

void HeliosEditor::RebuildCS()
{
    std::cout << "Rebuilding csharp..." << "\n";
    std::cout << "AAAAAAAAAAAAAAAAAAAAAA: " << 2 << "\n";

    // Scripting::ReloadService("CSharp", [&]() -> bool { return
    // m_buildDotNetScriptsTask.Run(); });

    Scripting::StopService("CSharp");
    m_buildDotNetScriptsTask.RunAsync([&](bool result)
                                      { Scripting::StartService("CSharp"); });
}

void HeliosEditor::OnUpdate()
{
    // static uint32 c = 0;
    // if (c++ > 10)
    // {
    //     std::terminate();
    // }

    if (rebuild_dotnet && currentMode == EditorMode::Editor)
    {
        std::cout << "AAAAAAAAAAAAAAAAAAAAAA: " << 1 << "\n";
        RebuildCS();
        rebuild_dotnet = false;
    }

    switch (currentMode)
    {
    case EditorMode::Playing:
        SceneRegistry::OnRuntimeUpdate();
        // rustScriptsManager->OnUpdate();
        break;
    default:
        SceneRegistry::OnEditorUpdate();
        break;
    }

    // if (InputManager::IsKeyPressed(HL_KEY_CONTROL) &&
    // InputManager::IsKeyPressed(HL_KEY_F10))
    //{
    //	DepricatedHelios::ShowMessage("INFO!", "Toggled GUI",
    //MB_ICONINFORMATION, false);
    //	//hideGui = !hideGui;
    // }

    // switch (currentRSState)
    //{
    // case RSState::Wireframe:
    //	graphics->m_deviceContext->RSSetState(graphics->wireframeRasterizerState.Get());
    //	break;
    // default:
    //	graphics->m_deviceContext->RSSetState(nullptr);
    //	break;
    // }

    if (isGameSceneActive)
    {
        // static std::vector<Ref<Framebuffer>> buffers = {gameFrame};

        // SceneRegistry::OnRuntimeRender(buffers);
    }

    // ScenePanel::Render(*SceneRegistry::get_current_scene().get());

    if (InputManager::IsKeyPressed(HL_KEY_CONTROL) &&
        InputManager::IsKeyPressed(HL_KEY_F10))
    {
        Helios::ShowMessage("INFO!", "Toggled GUI",
                            Message::Flags::IconInformation);
        m_barebones = !m_barebones;
    }

    
    if (InputManager::IsKeyPressed(HL_KEY_CONTROL) &&
        InputManager::IsKeyPressed(HL_KEY_F11))
    {
        Helios::ShowMessage("INFO!", "Toggled Per Frame Debug",
                            Message::Flags::IconInformation);
        m_perFrameDebug = !m_perFrameDebug;
    }

    if (InputManager::IsKeyPressed(HL_KEY_CONTROL) &&
        InputManager::IsKeyPressed(HL_KEY_F5))
    {
        RebuildCS();
    }

    if (!m_barebones) {
        OnGUI(); 
        Helios::Debug::CacheDebugger::OnImGuiRender();
    }

    if (m_perFrameDebug)
    {
        while (!(InputManager::IsKeyPressed(HL_KEY_CONTROL) &&
                 InputManager::IsKeyPressed(HL_KEY_N)))
        {
            if (InputManager::IsKeyPressed(HL_KEY_SHIFT))
            {
                m_perFrameDebug = false;
                break;
            }
        }
        Helios::ShowMessage("INFO!", "Next Frame",
                            Message::Flags::IconInformation);
    }
}

void HeliosEditor::OnRender(NativeCommandList& cmdList)
{
    // if (m_barebones)
    // {
    // 	Ref<Framebuffer> tmp = GetWindow()->GetContext().GetFramebuffer();
    // 	tmp->Bind();
    // 	// GetWindow()->GetContext().BindDefaultFramebuffer();
    // 	static EditorCamera camera;

    // 	Vector2 look = Vector3::Zero();

    // 	if (InputManager::IsKeyDown(HL_KEY_ARROW_UP))
    // 		look.x = -1;

    // 	if (InputManager::IsKeyDown(HL_KEY_ARROW_DOWN))
    // 		look.x = 1;

    // 	if (InputManager::IsKeyDown(HL_KEY_ARROW_RIGHT))
    // 		look.y = 1;

    // 	if (InputManager::IsKeyDown(HL_KEY_ARROW_LEFT))
    // 		look.y = -1;

    // 	if (InputManager::IsKeyDown(HL_KEY_R))
    // 		camera.Reset();

    // 	camera.HandleMovement(look);
    // 	camera.SetViewportSize({GetWindow()->GetWidth(),
    // GetWindow()->GetHeight()}); 	SceneRegistry::OnEditorRender(tmp, camera);
    // }
    // else
    // {

    // 	for (auto &panel : panels)
    // 	{
    // 		ScenePanel *tmp;

    // 		if ((tmp = dynamic_cast<ScenePanel *>(panel)) == nullptr)
    // 			continue;

    // 		tmp->RenderFramebuffer();
    // 	}
    // }

    for (auto& panel : panels)
    {
        ScenePanel* tmp;

        if ((tmp = dynamic_cast<ScenePanel*>(panel)) == nullptr)
            continue;

        tmp->RenderFramebuffer(cmdList);
    }


    static auto cmdBuff =
        CommandAllocator::GetForCurrentThread().CreateCommandListImmediate();

    auto passInfo = GetWindow()->GetContext().GetRenderPassBeginInfo();
    // cmdList.BeginRenderPass(passInfo);
    // GetWindow()->DrawImGui(cmdList);
    // cmdList.EndRenderPass();
    cmdBuff->Reset();
    cmdBuff->Begin();
    cmdBuff->BeginRenderPass(passInfo);
    GetWindow()->DrawImGui(*cmdBuff);
    cmdBuff->EndRenderPass();
    cmdBuff->End();
    cmdBuff->Execute();
}

void HeliosEditor::OnGUI()
{
    GetWindow()->NewImGuiFrame();

    // static ImGuiIO& io = ImGui::GetIO(); (void)io;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    static bool dock_open = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 10));
    ImGui::Begin("Dockspace Demon", &dock_open, window_flags);
    ImGui::PopStyleVar(4);

    static float bottom_bar_height = 25;
    float dockspace_height =
        ImGui::GetContentRegionAvail().y - bottom_bar_height;
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, dockspace_height),
                     dockspace_flags);

    if (ImGui::IsKeyReleased(ImGuiKey_S) &&
        ImGui::IsKeyReleased(ImGuiKey_ModCtrl))
        ImGui::IsKeyReleased(ImGuiKey_LeftCtrl))
        Project::SaveScene(ImGui::IsKeyReleased(ImGuiKey_LeftShift) ||
                           ImGui::IsKeyReleased(ImGuiKey_RightShift));

    DrawToolbar(currentMode, panels);

    bool show_metrics = true;
    ImGui::ShowMetricsWindow(&show_metrics);

    static bool showStyleEditor = true;

    /*if (showStyleEditor)
    {
            ImGui::Begin("Style Editor", &showStyleEditor);
            ImGui::ShowStyleEditor();
            ImGui::End();
    }*/

    ImGui::Begin("Colors");

    auto& colors = ImGui::GetStyle().Colors;
    ImGui::ColorEdit4("WindowBg", &colors[ImGuiCol_WindowBg].x);

    ImGui::ColorEdit4("FrameBg", &colors[ImGuiCol_FrameBg].x);
    ImGui::ColorEdit4("FrameBgHovered", &colors[ImGuiCol_FrameBgHovered].x);
    ImGui::ColorEdit4("FrameBgActive", &colors[ImGuiCol_FrameBgActive].x);
    ImGui::ColorEdit4("TitleBg", &colors[ImGuiCol_TitleBg].x);
    ImGui::ColorEdit4("TitleBgActive", &colors[ImGuiCol_TitleBgActive].x);
    ImGui::ColorEdit4("TitleBgCollapsed", &colors[ImGuiCol_TitleBgCollapsed].x);
    ImGui::ColorEdit4("MenuBarBg", &colors[ImGuiCol_MenuBarBg].x);
    ImGui::ColorEdit4("ScrollbarBg", &colors[ImGuiCol_ScrollbarBg].x);
    ImGui::ColorEdit4("ScrollbarGrab", &colors[ImGuiCol_ScrollbarGrab].x);
    ImGui::ColorEdit4("ScrollbarGrabHovered",
                      &colors[ImGuiCol_ScrollbarGrabHovered].x);
    ImGui::ColorEdit4("ScrollbarGrabActive",
                      &colors[ImGuiCol_ScrollbarGrabActive].x);
    ImGui::ColorEdit4("CheckMark", &colors[ImGuiCol_CheckMark].x);
    ImGui::ColorEdit4("SliderGrab", &colors[ImGuiCol_SliderGrab].x);
    ImGui::ColorEdit4("SliderGrabActive", &colors[ImGuiCol_SliderGrabActive].x);
    ImGui::ColorEdit4("Button", &colors[ImGuiCol_Button].x);
    ImGui::ColorEdit4("ButtonHovered", &colors[ImGuiCol_ButtonHovered].x);
    ImGui::ColorEdit4("ButtonActive", &colors[ImGuiCol_ButtonActive].x);
    ImGui::ColorEdit4("Header", &colors[ImGuiCol_Header].x);
    ImGui::ColorEdit4("HeaderHovered", &colors[ImGuiCol_HeaderHovered].x);
    ImGui::ColorEdit4("HeaderActive", &colors[ImGuiCol_HeaderActive].x);
    ImGui::ColorEdit4("Separator", &colors[ImGuiCol_Separator].x);
    ImGui::ColorEdit4("SeparatorHovered", &colors[ImGuiCol_SeparatorHovered].x);
    ImGui::ColorEdit4("SeparatorActive", &colors[ImGuiCol_SeparatorActive].x);
    ImGui::ColorEdit4("ResizeGrip", &colors[ImGuiCol_ResizeGrip].x);
    ImGui::ColorEdit4("ResizeGripHovered",
                      &colors[ImGuiCol_ResizeGripHovered].x);
    ImGui::ColorEdit4("ResizeGripActive", &colors[ImGuiCol_ResizeGripActive].x);
    ImGui::ColorEdit4("Tab", &colors[ImGuiCol_Tab].x);
    ImGui::ColorEdit4("TabHovered", &colors[ImGuiCol_TabHovered].x);
    ImGui::ColorEdit4("TabActive", &colors[ImGuiCol_TabActive].x);
    ImGui::ColorEdit4("TabUnfocused", &colors[ImGuiCol_TabUnfocused].x);
    ImGui::ColorEdit4("TabUnfocusedActive",
                      &colors[ImGuiCol_TabUnfocusedActive].x);

    ImGui::End();

    for (auto& panel : panels)
    {
        if (panel->m_window_open)
        {
            std::string tmp = panel->GetName();
            if (panel->IsCustomBegin())
            {
                HL_PROFILE_BEGIN(("GUI - " + tmp));
                panel->OnUpdate();
                HL_PROFILE_END();
            }
            else
            {
                if (ImGui::Begin(panel->GetName().c_str(),
                                 &panel->m_window_open, panel->GetFlags()))
                {
                    HL_PROFILE_BEGIN(("GUI - " + tmp));
                    panel->OnUpdate();
                    HL_PROFILE_END();
                }
                ImGui::End();
            }
        }
    }

    auto i = std::begin(panels);

    while (i != std::end(panels))
    {
        if ((*i)->destroy_window)
            i = panels.erase(i);
        else
            ++i;
    }

    AssetRegistry::ShowRegistryWindow();
    AssetRegistry::ShowTextureSelect();

    // HL_PROFILE_BEGIN("GUI - Project Explorer");
    //// ProjectExplorerWindow(Project::GetAssetsPath());
    // HL_PROFILE_END();

    ImGui::End();
}

} // namespace Helios