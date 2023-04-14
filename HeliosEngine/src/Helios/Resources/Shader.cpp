#include "Shader.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
	void Shader::Bind()
	{
		Graphics::instance->m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		Graphics::instance->m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
		if(m_geometryShader)
			Graphics::instance->m_deviceContext->GSSetShader(m_geometryShader.Get(), nullptr, 0u);
		
		Graphics::instance->m_deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_topology);
		//Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		Graphics::instance->m_deviceContext->IASetInputLayout(m_inputLayout.Get());
	}

	void Shader::Unbind()
	{
		Graphics::instance->m_deviceContext->VSSetShader(nullptr, nullptr, 0u);
		Graphics::instance->m_deviceContext->PSSetShader(nullptr, nullptr, 0u);
		if (m_geometryShader)
			Graphics::instance->m_deviceContext->GSSetShader(nullptr, nullptr, 0u);

		Graphics::instance->m_deviceContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)Topology::TriangleList);
		Graphics::instance->m_deviceContext->IASetInputLayout(nullptr);
	}
}
