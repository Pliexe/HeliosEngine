#include "Shader.h"
#include "Helios/Graphics/DepricatedGraphics.h"
#include "Helios/Graphics/Graphics.h"

#ifdef HELIOS_PLATFORM_WINDOWS
#include "Platform/Direct3D11/Shader/D3D11Shader.h"
#endif

namespace Helios
{
	void DepricatedShader::Bind()
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

	void DepricatedShader::Unbind()
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

 	D3D11_COMPARISON_FUNC GetDepthFuncNative2(DepthFunc func)
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

	void DepricatedShader::SetDepthFunc(DepthFunc func)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		m_depthStencilState->GetDesc(&depthStencilDesc);
		depthStencilDesc.DepthFunc = GetDepthFuncNative2(func);

		HRESULT hr;

		HL_EXCEPTION_HR(
			FAILED (hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)),
			"Failed to create depth stencil state!", hr
		);
	}

	#define DX_SHADER_EXTENSION ".hlsl"
	
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11Shader>(name, vertex_path + DX_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		}

		HELIOS_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11Shader>(name, vertex_path + DX_SHADER_EXTENSION, pixel_path + DX_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		}

		HELIOS_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11Shader>(name, vertex_path + DX_SHADER_EXTENSION, pixel_path + DX_SHADER_EXTENSION, geometry_path + DX_SHADER_EXTENSION, inputLayouts, depthFunc, topology);
		}

		HELIOS_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

}
