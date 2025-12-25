#pragma once

#include "Helios/Graphics/GPURenderPass.h"
#include "pch.h"

namespace Helios
{
	class Framebuffer;
	class NativeCommandList;
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
	
		virtual bool Init() = 0;
		virtual void SetVSync(uint32_t interval) = 0;
		
		virtual uint32_t GetVSyncInterval() = 0;
		
		virtual Ref<Image> GetSwapchainImage() const = 0;
		virtual const RenderPassBeginInfo GetRenderPassBeginInfo() const = 0;

		virtual void ResizeSwapchain(const Size& size) = 0;
		
		virtual void BindDefaultFramebuffer() = 0;
		
		virtual void WaitAllSync() = 0;
		
		virtual void Begin() = 0;
		virtual void End() = 0;
		
		virtual void SetViewport(uint32_t width, uint32_t height) = 0;

		virtual Ref<Framebuffer> GetFramebuffer() = 0;
	};
}
