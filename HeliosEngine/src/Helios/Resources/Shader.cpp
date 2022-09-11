#include "Shader.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
	void Shader::Bind()
	{
		Graphics::instance->m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		Graphics::instance->m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
		
		Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::instance->m_deviceContext->IASetInputLayout(m_inputLayout.Get());
	}
}