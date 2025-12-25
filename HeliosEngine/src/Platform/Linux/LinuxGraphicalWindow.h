#pragma once
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/GraphicsDevice.h"
#include "Platform/GLFW/GLFWManager.h"
#include "Platform/Vulkan/CommandList/VkCommandListImmediate.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "pch.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Core/GraphicalWindow.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include <Helios/Events/Events.h>
#include <memory>
#include "Platform/Vulkan/VkContext.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_vulkan.h>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <X11/Xatom.h>


#include <GLFW/glfw3.h>

#undef None

//#ifdef HELIOS_PLATFORM_LINUX
namespace Helios::Internal
{
#pragma region Window Dragging Functions
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
#pragma endregion
    
    class LinuxGraphicalWindow : public GraphicalWindow
    {
    public:
        void Show() override { glfwShowWindow(m_Window); }
        // void Hide() override { glfwHideWindow(m_Window); }

        Message::Result ShowMessage(const std::string& title, const std::string& message, Message::Flags type) const override;

        bool IsFocused() override { return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED); }

        

        void Update() override
        {
            // m_Context->End();
            glfwPollEvents();
        }

        std::string GetTitle() const override
        {
            return m_Title;
        }

        bool IsMaximized() const override
		{
			return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
		}

		void Restore() override
		{
			glfwRestoreWindow(m_Window);
		}

		void Maximize() override
		{
			glfwMaximizeWindow(m_Window);
		}

		void Minimize() override
		{
			glfwIconifyWindow(m_Window);
		}

        void Close() override {
            if (m_EventCallback) {
                WindowCloseEvent event;
                m_EventCallback(event);
            }
            glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
        }

        bool Create(Specifications specs) override
        {
            m_Title = specs.title;
			m_Style = specs.style;

            // Init GLFW if this is the first window
            if (s_WindowCount == 0)
            {
                if (!GLFWManager::Initialize()) return false;                
            }

            switch (Graphics::GetAPI())
            {
            case Graphics::API::OpenGL:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                break;
            case Graphics::API::Direct3D12:
                HL_ASSERT(false, "Direct3D12 is not supported on Linux");
                break;
            case Graphics::API::Direct3D11:
                HL_ASSERT(false, "Direct3D11 is not supported on Linux");
                break;
            case Graphics::API::Vulkan:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            // case Helios::Graphics::API::None:
            //     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            //     break;
            default: HL_EXCEPTION(true, "Uknown Graphics API!");
            }

            glfwWindowHint(GLFW_DECORATED, specs.style & WindowStyles::Decorated ? GLFW_TRUE : GLFW_FALSE);
            glfwWindowHint(GLFW_RESIZABLE, specs.style & WindowStyles::Resizable ? GLFW_TRUE : GLFW_FALSE);

            if (specs.style & WindowStyles::AlwaysOnTop)
                glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

            int count;
            int windowWidth, windowHeight;
            int monitorX, monitorY;

            GLFWmonitor** monitors = glfwGetMonitors(&count);
            const GLFWvidmode* videoMode = glfwGetVideoMode(monitors[0]);
            windowWidth = videoMode->width / 1.5;
            windowHeight = windowWidth / 16 * 9;

            glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);

            m_Width = specs.width;
            m_Height = specs.height;
            m_Window = glfwCreateWindow(specs.width, specs.height, specs.title.c_str(), nullptr, nullptr);

            if (!m_Window)
            {
                if (s_WindowCount == 0)
                    glfwTerminate();
                return false;
            }

            if (glfwGetPlatform() == GLFW_PLATFORM_X11 && m_Style & WindowStyles::NoTitlebar)
            {
                Display* display = glfwGetX11Display();
                ::Window x11Window = glfwGetX11Window(m_Window);

                if (!display) return false;

                struct MotifWmHints {
                    unsigned long flags;
                    unsigned long functions;
                    unsigned long decorations;
                    long inputMode;
                    unsigned long status;
                };

                enum {
                    MWM_HINTS_FUNCTIONS = (1L << 0),
                    MWM_HINTS_DECORATIONS = (1L << 1)
                };

                MotifWmHints hints;
                hints.flags = MWM_HINTS_DECORATIONS;
                hints.decorations = 0;

                Atom property = XInternAtom(display, "_MOTIF_WM_HINTS", False);
                XChangeProperty(
                    display,
                    x11Window,
                    property,
                    property,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&hints),
                    5
                );

                // Set _NET_WM_WINDOW_TYPE to _NET_WM_WINDOW_TYPE_NORMAL
                Atom netWmWindowType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
                Atom netWmWindowTypeNormal = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
                XChangeProperty(
                    display,
                    x11Window,
                    netWmWindowType,
                    XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&netWmWindowTypeNormal),
                    1
                );

                // Set _NET_WM_ALLOWED_ACTIONS to include move, resize, maximize, minimize
                Atom netWmAllowedActions = XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", False);
                Atom allowedActions[] = {
                    XInternAtom(display, "_NET_WM_ACTION_MOVE", False),
                    XInternAtom(display, "_NET_WM_ACTION_RESIZE", False),
                    XInternAtom(display, "_NET_WM_ACTION_MINIMIZE", False),
                    XInternAtom(display, "_NET_WM_ACTION_MAXIMIZE_HORZ", False),
                    XInternAtom(display, "_NET_WM_ACTION_MAXIMIZE_VERT", False),
                    XInternAtom(display, "_NET_WM_ACTION_FULLSCREEN", False),
                    XInternAtom(display, "_NET_WM_ACTION_CLOSE", False),
                };
                XChangeProperty(
                    display,
                    x11Window,
                    netWmAllowedActions,
                    XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(allowedActions),
                    sizeof(allowedActions) / sizeof(Atom)
                );

                XFlush(display);
            }


            glfwSetWindowPos(m_Window,
                monitorX + (videoMode->width - windowWidth) / 2,
                monitorY + (videoMode->height - windowHeight) / 2);

            if (specs.style & WindowStyles::Maximized)
                glfwMaximizeWindow(m_Window);

            if (specs.style & WindowStyles::Show)
                glfwShowWindow(m_Window);

            switch (Graphics::GetAPI())
            {
            case Graphics::API::OpenGL:
                m_Context = CreateUnique<OpenGLContext>(m_Window);
                break;
            case Graphics::API::Vulkan:
                m_Context = CreateUnique<VkContext>(std::static_pointer_cast<VkGraphicsDevice>(Graphics::GetMainDevice()), [&](){
                    int width, height;
                    glfwGetFramebufferSize(m_Window, &width, &height);
                    return Size(width, height);
                });
                break;
            case Graphics::API::Direct3D11:
                HL_ASSERT(false, "Direct3D11 not supported on Linux");
                break;
            default:
                HL_ASSERT(false, "Unsupported Graphics API!");
                break;
            }

            m_Context->Init();
            //m_Context->SetVSync(1);
            m_Context->SetVSync(0);

            glfwSetWindowUserPointer(m_Window, this);

            glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
                {
                    LinuxGraphicalWindow* win = (LinuxGraphicalWindow*)glfwGetWindowUserPointer(window);
                    if (win->m_EventCallback)
                    {
                        WindowCloseEvent event;
                        win->m_EventCallback(event);
                    }
                });

            HL_ASSERT(m_Window != nullptr, "Failed to create window!");


            glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                // Hello* win = (LinuxGraphicalWindow*)glfwGetWindowUserPointer(window);
                // win->m_Context->SetViewport(width, height);
                // win->m_Width = width;
                // win->m_Height = height;

                // if (win->m_EventCallback)
                // {
                //     WindowResizeEvent event(width, height);
                //     win->m_EventCallback(event);
                // }
            });

            glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
            glfwSetCursorPosCallback(m_Window, CursorPosCallback);

            return true;
        }


        void Destroy() override
        {
            if (useimgui) 
            {
                switch (Graphics::GetAPI())
                {
                    case Graphics::API::Vulkan:
                    {
                        ImGui_ImplVulkan_Shutdown();
                        ImGui_ImplGlfw_Shutdown();
                        ImGui::DestroyContext(m_ImGuiContext);

                        if (m_ImGuiDescriptorPool != VK_NULL_HANDLE)
                        {
                            vkDestroyDescriptorPool(reinterpret_cast<VkGraphicsDevice&>(*Graphics::GetMainDevice()).GetNativeDevice(), m_ImGuiDescriptorPool, nullptr);
                            m_ImGuiDescriptorPool = VK_NULL_HANDLE;
                        }
                        break;
                    }
                    default: HL_EXCEPTION(true, "Unknown graphics API!");
                }

                m_ImGuiContext = nullptr;
                useimgui = false;
            }
            
            glfwDestroyWindow(m_Window);
            --s_WindowCount;

            // TODO: Cleanup D3D11 if in use

            if (s_WindowCount == 0)
                glfwTerminate();
        }

        void* GetNativeWindow() const override { return m_Window; }

        void NewImGuiFrame() override
        {
            if (useimgui)
            {
                ImGui::SetCurrentContext(m_ImGuiContext);
                switch (Graphics::GetAPI())
                {
                case Graphics::API::OpenGL:
                    ImGui_ImplOpenGL3_NewFrame();
                    break;
                case Graphics::API::Vulkan:
                    ImGui_ImplVulkan_NewFrame();
                    break;
                default: HL_EXCEPTION(true, "No graphics API selected!");
                }
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
            }
        }

        void DrawImGui(NativeCommandListImmediate& commandList) override
        {
            if (!useimgui) return;
            
            
            ImGui::Render();
            switch (Graphics::GetAPI())
            {
                case Graphics::API::Direct3D11:
                {
                    HL_ASSERT(false, "Direct3D11 not supported on Linux");
                    break;
                }
                case Graphics::API::OpenGL:
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                break;
                case Graphics::API::Vulkan:
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VkCommandListImmediate&>(commandList).GetCommandBuffer());
                break;
                default: HL_EXCEPTION(true, "Unknown graphics API!");
            }
            
            static ImGuiIO& io = ImGui::GetIO();
            
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }

        GraphicsContext& GetContext() const override
        {
            return *m_Context;
        }

        std::string GetWindowTitle() const override
        {
            if (m_Window == nullptr) return "";
            return glfwGetWindowTitle(m_Window);
        }

        ImGuiContext* CreateImGuiContext() override
        {
            IMGUI_CHECKVERSION();
            std::cout << "ImGui Version: " << ImGui::GetVersion() << std::endl;
            m_ImGuiContext = ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
            //io.ConfigViewportsNoAutoMerge = true;
            //io.ConfigViewportsNoTaskBarIcon = true;

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();


            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
            return m_ImGuiContext;
        }

        void ImpImGui() override
        {
            switch (Graphics::GetAPI())
            {
            default:
                HL_EXCEPTION(true, "No graphics API selected!");
                break;
            case Graphics::API::Direct3D11:
            {
                HL_ASSERT(false, "Direct3D11 not supported on Linux");
            }
            case Graphics::API::OpenGL:
            {
                ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
                ImGui_ImplOpenGL3_Init("#version 410");
                break;
            }
            case Graphics::API::Vulkan:
                {
                    ImGui_ImplGlfw_InitForVulkan(m_Window, true);
                    
                    auto& vkContext = reinterpret_cast<VkContext&>(*m_Context);
                    auto& device = reinterpret_cast<VkGraphicsDevice&>(*Graphics::GetMainDevice());

                    if (m_ImGuiDescriptorPool == VK_NULL_HANDLE)
                    {
                        VkDescriptorPoolSize pool_sizes[] =
                        {
                            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
                        };

                        VkDescriptorPoolCreateInfo pool_info{};
                        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                        pool_info.maxSets = 1000 * std::size(pool_sizes);
                        pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
                        pool_info.pPoolSizes = pool_sizes;

                        HL_EXCEPTION(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_ImGuiDescriptorPool) != VK_SUCCESS,
                            "Failed to create ImGui descriptor pool!");
                    }

                    auto& vkCtx = static_cast<VkContext&>(*m_Context);
                    
                    ImGui_ImplVulkan_InitInfo init_info = {};
                    init_info.Instance = VkGraphicsDevice::GetVkInstance();            
                    init_info.PhysicalDevice = device.GetNativePhysicalDevice();
                    init_info.Device = device;
                    init_info.QueueFamily = vkContext.GetGraphicsQueueFamilyIndex();
                    init_info.Queue = device.GetGraphicsQueue();
                    init_info.PipelineCache = VK_NULL_HANDLE;
                    init_info.DescriptorPool = m_ImGuiDescriptorPool;
                    init_info.Subpass = 0;
                    init_info.MinImageCount = vkContext.GetSwapChain().GetMinImageCount();
                    init_info.ImageCount = vkContext.GetSwapChain().GetSwapchainImageCount();
                    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
                    init_info.Allocator = nullptr;
                    init_info.CheckVkResultFn = nullptr;
                    init_info.RenderPass = vkCtx.GetNativeRenderPass();
                    
                    ImGui_ImplVulkan_Init(&init_info);

                    // ImGui_ImplVulkan_CreateFontsTexture();

                    // ImGui_ImplVulkan_DestroyFontUploadObjects();
                    
                }
                break;
            }

            useimgui = true;
        }


        ImGuiContext* GetImGuiContext() const override
        {
            return m_ImGuiContext;
        }

        void SetVSync(bool enabled) override;
		bool VSyncEnabled() const override;
        void SetTitle(const std::string& title) override;
        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }

        Point GetPosition() const override
		{
			Point point;
			glfwGetWindowPos(m_Window, &point.x, &point.y);
			return point;
		}

		void SetPosition(const Point &pos) override
		{
			glfwSetWindowPos(m_Window, pos.x, pos.y);
			// if (m_Style & WindowStyles::Resizable && !(m_Style & WindowStyles::Decorated))
			// {
			// 	HWND hWnd = glfwGetWin32Window(m_Window);

			// 	RECT windowRect;
			// 	GetClientRect(hWnd, &windowRect);
			// 	AdjustWindowRect(&windowRect, WS_THICKFRAME, FALSE);
			// 	SetWindowPos(hWnd, NULL, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			// }
		}

        LinuxGraphicalWindow() { InitCursors(); }
        ~LinuxGraphicalWindow() { DestroyCursors(); }

    private:
        GLFWwindow* m_Window = nullptr;
        Unique<GraphicsContext> m_Context;
        ImGuiContext* m_ImGuiContext;
        bool useimgui = false;
        uint32_t m_Width, m_Height;
		WindowStyles::Value m_Style;
        VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;


#pragma region Window Dragging

        friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        friend void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

        void StartWindowMove(double mouseX, double mouseY);

bool dragging = false;
double dragOffsetX = 0, dragOffsetY = 0;
#pragma endregion

#pragma region Window Resizing
    enum class ResizeEdge {
        None = 0,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    ResizeEdge currentResizeEdge = ResizeEdge::None;
    bool resizing = false;
    double resizeDragStartX = 0.0, resizeDragStartY = 0.0;
    int resizeStartWinX = 0, resizeStartWinY = 0;
    int resizeStartWinWidth = 0, resizeStartWinHeight = 0;

    
    GLFWcursor* arrowCursor = nullptr;
    GLFWcursor* resizeNWSE = nullptr;
    GLFWcursor* resizeNESW = nullptr;
    GLFWcursor* resizeEW   = nullptr;
    GLFWcursor* resizeNS   = nullptr;

    void InitCursors() {
        arrowCursor   = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        resizeNWSE    = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
        resizeNESW    = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
        resizeEW      = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
        resizeNS      = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    }

    void DestroyCursors() {
        if (arrowCursor)   glfwDestroyCursor(arrowCursor);
        if (resizeNWSE)    glfwDestroyCursor(resizeNWSE);
        if (resizeNESW)    glfwDestroyCursor(resizeNESW);
        if (resizeEW)      glfwDestroyCursor(resizeEW);
        if (resizeNS)      glfwDestroyCursor(resizeNS);
    }

#pragma endregion // Reisizing

        inline static uint32_t s_WindowCount = 0;
        Ref<GraphicsDevice> m_graphicsDevice;
    };
}
//#endif