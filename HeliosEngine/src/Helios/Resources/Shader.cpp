#include "Shader.h"
#include "Helios/Graphics/Graphics.h"

#ifdef HELIOS_PLATFORM_WINDOWS
#include "Platform/Direct3D11/Shader/D3D11Shader.h"
#endif
#include "Platform/Vulkan/Shader/VkShader.h"

namespace Helios
{

	#define DX_SHADER_EXTENSION ".hlsl"
	#define VK_SHADER_EXTENSION ".spv"
	
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11Shader>(name, vertex_path + DX_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		case Graphics::API::Vulkan: return CreateRef<VkShader>(name, vertex_path + VK_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11Shader>(name, vertex_path + DX_SHADER_EXTENSION, pixel_path + DX_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		case Graphics::API::Vulkan: return CreateRef<VkShader>(name, vertex_path + VK_SHADER_EXTENSION, pixel_path + VK_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11Shader>(name, vertex_path + DX_SHADER_EXTENSION, pixel_path + DX_SHADER_EXTENSION, geometry_path + DX_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		case Graphics::API::Vulkan: return CreateRef<VkShader>(name, vertex_path + VK_SHADER_EXTENSION, pixel_path + VK_SHADER_EXTENSION, geometry_path + VK_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

}
