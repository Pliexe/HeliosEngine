#include "Shader.h"
#include "Helios/Graphics/DepricatedGraphics.h"

namespace Helios
{
	void Shader::Bind()
	{
		const auto& context = Direct3D11Context::GetCurrentContext()->GetContext();

		context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
		if(m_geometryShader)
			context->GSSetShader(m_geometryShader.Get(), nullptr, 0u);
		
		context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_topology);
		context->IASetInputLayout(m_inputLayout.Get());
		context->OMSetDepthStencilState(m_depthStencilState.Get(), 1u);
		
	}

	void Shader::Unbind()
	{
		const auto& context = Direct3D11Context::GetCurrentContext()->GetContext();


		context->VSSetShader(nullptr, nullptr, 0u);
		context->PSSetShader(nullptr, nullptr, 0u);
		if (m_geometryShader)
			context->GSSetShader(nullptr, nullptr, 0u);

		context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)Topology::TriangleList);
		context->IASetInputLayout(nullptr);
		context->OMSetDepthStencilState(nullptr, 1u);
	}

 	D3D11_COMPARISON_FUNC GetDepthFuncNative(DepthFunc func)
	{
		switch (func)
		{
			case DepthFunc::Always: return D3D11_COMPARISON_ALWAYS;
			case DepthFunc::Never: return D3D11_COMPARISON_NEVER;
			case DepthFunc::Less: return D3D11_COMPARISON_LESS;
			case DepthFunc::LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
			case DepthFunc::Greater: return D3D11_COMPARISON_GREATER;
			case DepthFunc::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
			case DepthFunc::Equal: return D3D11_COMPARISON_EQUAL;
			case DepthFunc::NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
		}
	}

	void Shader::SetDepthFunc(DepthFunc func)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		m_depthStencilState->GetDesc(&depthStencilDesc);
		depthStencilDesc.DepthFunc = GetDepthFuncNative(func);

		HRESULT hr;

		HL_EXCEPTION_HR(
			FAILED (hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)),
			"Failed to create depth stencil state!", hr
		);
	}
}
