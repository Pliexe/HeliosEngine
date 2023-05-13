#include "Shader.h"
#include "Helios/Graphics/DepricatedGraphics.h"

namespace Helios
{
	void Shader::Bind()
	{
		Direct3D11Context::GetCurrentContext()->GetContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
		if(m_geometryShader)
			Direct3D11Context::GetCurrentContext()->GetContext()->GSSetShader(m_geometryShader.Get(), nullptr, 0u);
		
		Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_topology);
		//Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		Direct3D11Context::GetCurrentContext()->GetContext()->IASetInputLayout(m_inputLayout.Get());
	}

	void Shader::Unbind()
	{
		Direct3D11Context::GetCurrentContext()->GetContext()->VSSetShader(nullptr, nullptr, 0u);
		Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShader(nullptr, nullptr, 0u);
		if (m_geometryShader)
			Direct3D11Context::GetCurrentContext()->GetContext()->GSSetShader(nullptr, nullptr, 0u);

		Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)Topology::TriangleList);
		Direct3D11Context::GetCurrentContext()->GetContext()->IASetInputLayout(nullptr);
	}
}
