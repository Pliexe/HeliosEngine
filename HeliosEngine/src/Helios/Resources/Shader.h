#pragma once

#include "pch.h"
#include "Helios/Core/Application.h"
#include "Helios/Core/Asserts.h"
#include <d3dcompiler.h>

namespace Helios {
	class Shader
	{
	public:

		enum class DataType
		{
			Float2 = DXGI_FORMAT_R32G32_FLOAT,
			Float3 = DXGI_FORMAT_R32G32B32_FLOAT,
			Float4 = DXGI_FORMAT_R32G32B32A32_FLOAT,

			UInt8 = DXGI_FORMAT_R8G8B8A8_UINT,
			UInt8Norm = DXGI_FORMAT_R8G8B8A8_UNORM,
		};

		struct ShaderElement
		{
			const char* name;
			DataType type;
		};

		Shader() = delete;
		
		//Shader(std::string filepath, const std::initializer_list<ShaderElement> elements)
		template <std::size_t N>
		Shader(std::string filepath, const ShaderElement(&elements)[N])
		{
			Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

			std::string pixelShaderPath = (filepath + "PixelShader.cso");
			
			HL_EXCEPTION(
				FAILED(D3DReadFileToBlob(std::wstring(pixelShaderPath.begin(), pixelShaderPath.end()).c_str(), &pBlob)),
				"Failed to read pixel shader file!\n" + pixelShaderPath
			);

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pixelShader)),
				"Failed to create pixel shader!\n" + pixelShaderPath
			);

			std::string vertexShaderPath = (filepath + "VertexShader.cso");

			HL_EXCEPTION(
				FAILED(D3DReadFileToBlob(std::wstring(vertexShaderPath.begin(), vertexShaderPath.end()).c_str(), &pBlob)),
				"Failed to read vertex shader file!\n" + vertexShaderPath
			);

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_vertexShader)),
				"Failed to create vertex shader!\n" + vertexShaderPath
			);

			D3D11_INPUT_ELEMENT_DESC ied[N]{};

			for (int i = 0; i < N; i++)
			{
				ied[i] = {
					elements[i].name,
					0,
					(DXGI_FORMAT)elements[i].type,
					0,
					D3D11_APPEND_ALIGNED_ELEMENT,
					D3D11_INPUT_PER_VERTEX_DATA,
					0
				};
			}

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)N, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_inputLayout)),
				"Failed to create input layout!\n" + vertexShaderPath
			);
		}
		Shader(const Shader& other) = default;


		void Bind();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};
}