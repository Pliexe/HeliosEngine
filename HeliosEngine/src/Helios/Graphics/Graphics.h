#pragma once

namespace Helios
{
	class Graphics
	{
	public:
		enum class API
		{
			None,
			Direct3D11,
			Direct3D12,
			OpenGL,
			Vulkan,
		};

		static bool Initialize(API api);
		static void Shutdown();

		static API GetAPI() { return m_api; }

	private:
		inline static API m_api = API::None;

		friend class Application;
	};
}