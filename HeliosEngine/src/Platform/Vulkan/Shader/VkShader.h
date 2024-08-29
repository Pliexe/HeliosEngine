#pragma once

#include "pch.h"
#include "Helios\Resources\Shader.h"
#include "Helios\Resources\ShaderBuilder.h"

#include <vulkan/vulkan.h>

namespace Helios
{
	class VkShader : public Shader
	{
	public:
		VkShader(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
		VkShader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);
		VkShader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology);

		const std::string& GetName() const override { return m_name; }
		const DepthFunc GetDepthFunc() const override { return m_depthFunc; }
		const Topology GetTopology() const override { return m_topology; }

		void SetDepthFunc(DepthFunc depthFunc) override { m_depthFunc = depthFunc; }
		void SetTopology(Topology topology) override { m_topology = topology; }

		void Bind() const override;
		void Unbind() const override;

		const Helios::InputLayouts& GetInputLayouts() const override { return m_inputLayouts; }
	private:
		std::string m_name;

		InputLayouts m_inputLayouts;
		DepthFunc m_depthFunc = DepthFunc::Less;
		Topology m_topology = Topology::TriangleList;

		VkPipeline m_pipeline;
		VkShaderModule m_vertexShader = VK_NULL_HANDLE;
		VkShaderModule m_pixelShader = VK_NULL_HANDLE;
		VkShaderModule m_geometryShader = VK_NULL_HANDLE;
	};
}