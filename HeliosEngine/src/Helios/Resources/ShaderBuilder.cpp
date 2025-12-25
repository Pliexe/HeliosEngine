#include "ShaderBuilder.h"

namespace Helios
{
	// void ShaderBuilder::AddInputLayout(const InputLayout& inputLayout)
	// {
	// 	m_InputLayouts.AddLayout(inputLayout);
	// }

	// void ShaderBuilder::SetInputLayouts(const InputLayouts inputLayouts)
	// {
	// 	m_InputLayouts = inputLayouts;
	// }

	// void ShaderBuilder::SetName(const std::string& name) { m_Name = name; }
	// void ShaderBuilder::SetVertexShader(const std::string& vertexShader) { m_VertexShader = vertexShader; }
	// void ShaderBuilder::SetPixelShader(const std::string& pixelShader) { m_PixelShader = pixelShader; }
	// void ShaderBuilder::SetGeometryShader(const std::string& geometryShader) { m_GeometryShader = geometryShader; }
	// void ShaderBuilder::SetDepthFunc(DepthFunc depthFunc) { m_DepthFunc = depthFunc; }
	// void ShaderBuilder::SetTopology(Topology topology) { m_Topology = topology; }

	// Ref<DepricatedShader> ShaderBuilder::Create()
	// {
	// 	if (m_Name.empty())
	// 	{
	// 		m_Name = "Unnamed";
	// 	}

	// 	if (m_VertexShader.empty()) assert(true);
		
	// 	if(m_PixelShader.empty())
	// 		return DepricatedShader::Create(m_Name, m_VertexShader, m_InputLayouts, m_DepthFunc, m_Topology);
	// 	else if (m_GeometryShader.empty())
	// 		return DepricatedShader::Create(m_Name, m_VertexShader, m_PixelShader, m_InputLayouts, m_DepthFunc, m_Topology);
	// 	else
	// 		return DepricatedShader::Create(m_Name, m_VertexShader, m_PixelShader, m_GeometryShader, m_InputLayouts, m_DepthFunc, m_Topology);
	// }
}
