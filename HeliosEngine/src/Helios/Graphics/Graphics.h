#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Graphics/NativeCommandList.h"
#include "pch.h"
#include "Helios/Graphics/GraphicsDevice.h"

namespace Helios
{
	class RenderGraph;
	class HELIOS_API Graphics
	{
	public:
		enum class API
		{
			None,
			Direct3D11,
			Direct3D12,
			OpenGL,
			Vulkan,
			WebGPU,
			WebGL,
		};
		
		static API GetAPI() { return s_api; }
		
		static Ref<GraphicsDevice> GetMainDevice() { return s_mainDevice; }

		static uint8 GetMaxFramesInFlight() { return s_maxFramesInFlight; }
		static uint8 GetCurrentFrameIndex();
		static uint8 GetNextFrameIndex(); // Returns the index of the frame that is to be set on next frame

		static NativeCommandList& GetMainCommandList();
		static Ref<RenderGraph> GetMainRenderGraph();
		
		static uint64 GetFrameCount() { return s_frameCount; }

	private:
		static bool Initialize(API api);
		static void Shutdown();

		static void Begin();
		static void End();

		inline static API s_api = API::None;
		
		inline static Ref<GraphicsDevice> s_mainDevice;
		inline static Ref<RenderGraph> s_mainRenderGraph;
		inline static std::vector<Ref<NativeCommandList>> s_mainCommandLists;
		
		inline static uint8 s_maxFramesInFlight = 1u;
    	inline static uint8 s_currentFrameIndex = 0u;
		inline static uint64 s_frameCount = 0u;

		friend class Application;
	};
}

#include "Helios/Graphics/RenderGraph/RenderGraph.h"