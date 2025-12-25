#pragma once
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/GraphicalWindow.h"
#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Graphics/GraphicsContext.h"
#include "Helios/Core/Asserts.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Helios
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(const OpenGLContext &) = default;
		OpenGLContext(OpenGLContext &&) = default;
		OpenGLContext &operator=(const OpenGLContext &) = default;
		OpenGLContext &operator=(OpenGLContext &&) = default;

		Ref<Image> GetSwapchainImage() const override { return nullptr; }
		const RenderPassBeginInfo GetRenderPassBeginInfo() const override { return RenderPassBeginInfo {}; }

		OpenGLContext(GLFWwindow *windowHandle)
			: m_WindowHandle(windowHandle) {
		HL_ASSERT(m_WindowHandle, "GLFW Window handle is null!");
		}

		void BindDefaultFramebuffer() override
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void WaitAllSync() override {}
		
		
		void ResizeSwapchain(const Size& size) override { }

		bool Init() override
		{
			glfwMakeContextCurrent(m_WindowHandle);

			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			HL_ASSERT(status, "Failed to initialize Glad!");

			HL_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Helios requires at least OpenGL version 4.5!");

			return true;
		}

		void End() override
		{
			glfwSwapBuffers(m_WindowHandle);
			// Clear render buffer
			//glClearColor(1, 0, 0, 1); // debug, it works!!!
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void SetVSync(uint32_t interval) override
		{
			glfwSwapInterval(m_SwapInterval = interval);
		}

		uint32_t GetVSyncInterval() override
		{
			return m_SwapInterval;
		}

		void Begin() override
		{
			glfwMakeContextCurrent(m_WindowHandle);
			HL_EXCEPTION(true, "Missing CommandList, Not Implemented Yet!");
		}

		void SetViewport(uint32_t width, uint32_t height) override
		{
			glViewport(0, 0, width, height);
		}

		Ref<Framebuffer> GetFramebuffer() override
		{
			assert(false);
			return nullptr;
		}
	private:
		GLFWwindow* m_WindowHandle;
		uint32_t m_SwapInterval = 0u;
	};
}
