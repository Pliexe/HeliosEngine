#pragma once
#include "Helios/Core/Asserts.h"
#include "pch.h"
#include <string>

#ifdef HELIOS_PLATFORM_WINDOWS

#include "Helios/Core/GraphicalWindow.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Graphics/GraphicsContext.h"
#include "Platform/Direct3D11/Direct3D11Context.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VkContext.h"
#include "Helios/Math/Vector.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_vulkan.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Helios/Events/Events.h"
#include "imgui.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <dwmapi.h>

namespace Helios
{
	class HELIOS_API Win32GraphicalWindow : public GraphicalWindow
	{
	private:
		static inline WNDPROC s_original_proc = nullptr;

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_NCCALCSIZE:
			{
				// Remove the window's standard sizing border
				if (wParam == TRUE && lParam != NULL)
				{
					NCCALCSIZE_PARAMS *pParams = reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam);
					pParams->rgrc->top = pParams->rgrc->top + 1;
					pParams->rgrc->right = pParams->rgrc->right - 2;
					pParams->rgrc->bottom -= 2;
					pParams->rgrc->left += 2;
				}
				return 0;
			}
			case WM_NCLBUTTONDBLCLK:
			{
				if (wParam == HTCAPTION)
				{
					if (::IsMaximized(hWnd))
					SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
					else
					SendMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
					return 0;
				}
				break;
			}
			case WM_NCHITTEST:
			{
				const int borderWidth = 4;
				POINTS mousePos = MAKEPOINTS(lParam);
				POINT clientMousePos = {mousePos.x, mousePos.y};
				ScreenToClient(hWnd, &clientMousePos);
				RECT windowRect;
				GetClientRect(hWnd, &windowRect);

				if (clientMousePos.y >= windowRect.bottom - borderWidth)
				{
					if (clientMousePos.x <= borderWidth)
						return HTBOTTOMLEFT;
					else if (clientMousePos.x >= windowRect.right - borderWidth)
						return HTBOTTOMRIGHT;
					else
						return HTBOTTOM;
				}
				else if (clientMousePos.y <= borderWidth)
				{
					if (clientMousePos.x <= borderWidth)
						return HTTOPLEFT;
					else if (clientMousePos.x >= windowRect.right - borderWidth)
						return HTTOPRIGHT;
					else
						return HTTOP;
				}
				else if (clientMousePos.x <= borderWidth)
				{
					return HTLEFT;
				}
				else if (clientMousePos.x >= windowRect.right - borderWidth)
				{
					return HTRIGHT;
				}

				int titleBarHeight = 35;

				if (clientMousePos.y <= titleBarHeight) 
				{
					if (ImGui::IsAnyItemHovered()) {
						break;
					}
					
					return HTCAPTION;
				}

				break;
			}
			}

			return CallWindowProc(s_original_proc, hWnd, uMsg, wParam, lParam);
		}

	public:
		Message::Result ShowMessage(const std::string &title, const std::string &message, Message::Flags type = Message::Flags::Ok | Message::Flags::IconInformation) const override
		{
			UINT uType = NULL;

			if (type & Message::Flags::Ok)
				uType |= MB_OK;
			if (type & Message::Flags::OkCancel)
				uType |= MB_OKCANCEL;
			if (type & Message::Flags::AbortRetryIgnore)
				uType |= MB_ABORTRETRYIGNORE;
			if (type & Message::Flags::YesNoCancel)
				uType |= MB_YESNOCANCEL;
			if (type & Message::Flags::YesNo)
				uType |= MB_YESNO;

			if (type & Message::Flags::IconInformation)
				uType |= MB_ICONINFORMATION;
			if (type & Message::Flags::IconWarning)
				uType |= MB_ICONWARNING;
			if (type & Message::Flags::IconError)
				uType |= MB_ICONERROR;
			if (type & Message::Flags::IconQuestion)
				uType |= MB_ICONQUESTION;

			HRESULT hr = ::MessageBoxW(glfwGetWin32Window(m_Window), conversions::from_utf8_to_utf16(message).c_str(), conversions::from_utf8_to_utf16(title).c_str(), uType);

			switch (hr)
			{
			case IDOK:
				return Message::Result::Ok;
			case IDCANCEL:
				return Message::Result::Cancel;
			case IDABORT:
				return Message::Result::Abort;
			case IDRETRY:
				return Message::Result::Retry;
			case IDIGNORE:
				return Message::Result::Ignore;
			case IDYES:
				return Message::Result::Yes;
			case IDNO:
				return Message::Result::No;
			default:
				return Message::Result::None;
			}
		}

		void Show() override
		{
			glfwShowWindow(m_Window);
		}

		bool IsFocused() override
		{
			return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
		}

		std::string GetTitle() const override
		{
			return m_Title;
		}

		void Update() override
		{
			m_Context->SwapBuffers();
			glfwPollEvents();
		}

		bool Create(Specifications specs) override
		{
			m_Title = specs.title;
			m_Style = specs.style;
			// Init GLFW if this is the first window
			if (s_WindowCount == 0)
			{
				if (!glfwInit())
					return false;
			}

			switch (Graphics::GetAPI())
			{
			case Graphics::API::OpenGL:
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				break;
			case Graphics::API::Direct3D11:
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				break;
			case Graphics::API::Vulkan:
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				break;
			}

			glfwWindowHint(GLFW_DECORATED, specs.style & WindowStyles::Decorated ? GLFW_TRUE : GLFW_FALSE);
			glfwWindowHint(GLFW_RESIZABLE, specs.style & WindowStyles::Resizable ? GLFW_TRUE : GLFW_FALSE);
			glfwWindowHint(GLFW_MAXIMIZED, specs.style & WindowStyles::Maximized ? GLFW_TRUE : GLFW_FALSE);

			if (specs.style & WindowStyles::AlwaysOnTop)
				glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

			glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

			int count;
			int windowWidth, windowHeight;
			int monitorX, monitorY;

			GLFWmonitor **monitors = glfwGetMonitors(&count);
			const GLFWvidmode *videoMode = glfwGetVideoMode(monitors[0]);
			windowWidth = videoMode->width / 1.5;
			windowHeight = windowWidth / 16 * 9;

			glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);

			m_Width = specs.width;
			m_Height = specs.height;
			m_Window = glfwCreateWindow(specs.width, specs.height, specs.title.c_str(), nullptr, s_MainWindow != this ? static_cast<Win32GraphicalWindow *>(s_MainWindow)->m_Window : nullptr);

			if (!m_Window)
			{
				if (s_WindowCount == 0)
					glfwTerminate();
				return false;
			}

			if (m_Style & WindowStyles::NoTitlebar)
			{
				HWND hWnd = glfwGetWin32Window(m_Window);

				s_original_proc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
				(WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Win32GraphicalWindow::WindowProc));


				// LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
				// style |= WS_THICKFRAME;
				// style &= ~WS_CAPTION;
				// SetWindowLongPtr(hWnd, GWL_STYLE, style);

				// DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
				// DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

				MARGINS margins = {-1};
				DwmExtendFrameIntoClientArea(hWnd, &margins);
				RECT windowRect;
				GetClientRect(hWnd, &windowRect);
				AdjustWindowRect(&windowRect, WS_THICKFRAME, FALSE);
				SetWindowPos(hWnd, NULL, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
				// GetWindowRect(hWnd, &windowRect);
				// SetWindowPos(hWnd, NULL, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - 	windowRect.top, SWP_FRAMECHANGED | SWP_NOMOVE);

				// int trueValue = 0x01;
				// DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &trueValue, sizeof(trueValue));
			}

			if (specs.style & WindowStyles::Centered)
			{
				glfwSetWindowPos(m_Window,
								 monitorX + (videoMode->width - windowWidth) / 2,
								 monitorY + (videoMode->height - windowHeight) / 2);
			}

			if (specs.style & WindowStyles::Show)
				glfwShowWindow(m_Window);

			switch (Graphics::GetAPI())
			{
			case Graphics::API::OpenGL:
				m_Context = CreateScope<OpenGLContext>(m_Window);
				break;
			case Graphics::API::Direct3D11:
				m_Context = CreateScope<Direct3D11Context>(glfwGetWin32Window(m_Window));
				break;
			case Graphics::API::Vulkan:
				m_Context = CreateScope<VkContext>(std::static_pointer_cast<VkContext>(Graphics::GetMainDevice()));
				break;
			default:
				HL_ASSERT(false, "Unsupported Graphics API!");
				break;
			}

			m_Context->Init();
			// m_Context->SetVSync(1);
			m_Context->SetVSync(0);

			glfwSetWindowUserPointer(m_Window, this);

			glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
									   {
				Win32GraphicalWindow* win = (Win32GraphicalWindow*)glfwGetWindowUserPointer(window);
				if (win->m_EventCallback)
				{
					WindowCloseEvent event;
					win->m_EventCallback(event);
				} });

			glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
										   {
				Win32GraphicalWindow* win = (Win32GraphicalWindow*)glfwGetWindowUserPointer(window);
				if (width <= 0 || height <= 0) return;
				win->m_Context->SetViewport(width, height);
				win->m_Width = width;
				win->m_Height = height;

				if (win->m_EventCallback)
				{
					WindowResizeEvent event(width, height);
					win->m_EventCallback(event);
				} });

			return true;
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

		void Close() override
		{
			HWND hWnd = glfwGetWin32Window(m_Window);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			// glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
		}

		void Destroy() override
		{
			glfwDestroyWindow(m_Window);
			--s_WindowCount;

			// TODO: Cleanup D3D11 if in use

			if (s_WindowCount == 0)
				glfwTerminate();
		}

		void *GetNativeWindow() const override { return m_Window; }

		void BeginFrame() override
		{
			m_Context->UseContext();
			if (useimgui)
			{
				ImGui::SetCurrentContext(m_ImGuiContext);
				switch (Graphics::GetAPI())
				{
				case Graphics::API::Direct3D11:
					ImGui_ImplDX11_NewFrame();
					break;
				case Graphics::API::OpenGL:
					ImGui_ImplOpenGL3_NewFrame();
					break;
				case Graphics::API::Vulkan:
					ImGui_ImplVulkan_NewFrame();
					break;
				}
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
			}
		}

		void EndFrame() override
		{
			static ImGuiIO &io = ImGui::GetIO();

			if (useimgui)
			{
				ImGui::EndFrame();
				ImGui::Render();
				switch (Graphics::GetAPI())
				{
				case Graphics::API::Direct3D11:
				{
					auto ctx = static_cast<Direct3D11Context &>(GetContext());
					ctx.GetFramebuffer()->Bind();
					// ctx.GetContext()->OMSetRenderTargets(1, ctx.GetRenderTargetView().GetAddressOf(), nullptr);
					ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
					break;
				}
				case Graphics::API::OpenGL:
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
					break;
				case Graphics::API::Vulkan:
					// ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), GetContext()->GetCommandBuffer());
					break;
				}

				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					GLFWwindow *backup_current_context = glfwGetCurrentContext();
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
					glfwMakeContextCurrent(backup_current_context);
				}
			}

			m_Context->SwapBuffers();
		}

		GraphicsContext &GetContext() const override
		{
			return *m_Context;
		}

		ImGuiContext *CreateImGuiContext() override
		{
			IMGUI_CHECKVERSION();
			m_ImGuiContext = ImGui::CreateContext();
			ImGuiIO &io = ImGui::GetIO();
			(void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
			// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	// Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
			// io.ConfigViewportsNoAutoMerge = true;
			// io.ConfigViewportsNoTaskBarIcon = true;

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
			ImGuiStyle &style = ImGui::GetStyle();
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
			case Graphics::API::Direct3D11:
			{
				auto ctx = static_cast<Direct3D11Context &>(GetContext());

				ImGui_ImplGlfw_InitForOther(m_Window, true);
				// ImGui_ImplWin32_Init(glfwGetWin32Window(m_Window));
				ImGui_ImplDX11_Init(ctx.GetDevice().Get(), ctx.GetContext().Get());
				break;
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
				break;
			}
			}

			useimgui = true;
		}

		ImGuiContext *GetImGuiContext() const override
		{
			return m_ImGuiContext;
		}

		void SetVSync(bool enabled) override;
		bool VSyncEnabled() const override;
		void SetTitle(const std::string &title) override;
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

	private:
		GLFWwindow *m_Window;
		Scope<GraphicsContext> m_Context;
		ImGuiContext *m_ImGuiContext;
		bool useimgui = false;
		uint32_t m_Width, m_Height;
		WindowStyles::Value m_Style;

		inline static uint32_t s_WindowCount = 0;
	};
}

#endif