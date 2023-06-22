#pragma once

namespace Helios
{
	class GraphicsContext
	{
	public:
		virtual bool Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void SetVSync(uint32_t interval) = 0;

		virtual void BindDefaultFramebuffer() = 0;

		virtual void UseContext() = 0;
		virtual void SetViewport(uint32_t width, uint32_t height) = 0;
	};
}
