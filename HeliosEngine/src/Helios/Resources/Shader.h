#pragma once

#include "pch.h"
#include "Helios/Core/Application.h"
#include "Helios/Core/Asserts.h"
#include <d3dcompiler.h>

namespace Helios {
	class HELIOS_API Shader
	{
	public:

		enum class DataType
		{
			Float2 = DXGI_FORMAT_R32G32_FLOAT,
			Float3 = DXGI_FORMAT_R32G32B32_FLOAT,
			Float4 = DXGI_FORMAT_R32G32B32A32_FLOAT,

			Vector2 = DXGI_FORMAT_R32G32_FLOAT,
			Vector3 = DXGI_FORMAT_R32G32B32_FLOAT,
			Vector4 = DXGI_FORMAT_R32G32B32A32_FLOAT,

			UInt8 = DXGI_FORMAT_R8_UINT,
			UInt16 = DXGI_FORMAT_R16_UINT,
			UInt32 = DXGI_FORMAT_R32_UINT,

			Int8 = DXGI_FORMAT_R8_SINT,
			Int16 = DXGI_FORMAT_R16_SINT,
			Int32 = DXGI_FORMAT_R32_SINT,
			

			UInt8_8 = DXGI_FORMAT_R8G8B8A8_UINT,
			UInt8_8Norm = DXGI_FORMAT_R8G8B8A8_UNORM,

			Matrix4x4,
		};

		struct ShaderElement
		{
			enum class InputClassification {
				PerVertex = 0,
				PerInstance = 1
			};

			const char* name;
			DataType type;
			uint32_t slot = 0u;
			uint32_t instanceStepRate = 0u;
			InputClassification inputClassification = InputClassification::PerVertex;
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

			int size = 0;

			for (int i = 0; i < N; i++)
				if (elements[i].type == DataType::Matrix4x4)
					size+=4;
				else size++;
				
			D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[size];

			unsigned int is = 0;
			unsigned int index = 0;
			for (int i = 0; i < N; i++)
			{
				if (elements[i].type == DataType::Matrix4x4) {
					ied[index] = {
						elements[i].name,
						is,
						DXGI_FORMAT_R32G32B32A32_FLOAT,
						elements[i].slot,
						D3D11_APPEND_ALIGNED_ELEMENT,
						(D3D11_INPUT_CLASSIFICATION)elements[i].inputClassification,
						elements[i].instanceStepRate
					};
					is++;
					if(is < 4)
						i--;
				}
				else {
					is = 0;
					ied[index] = {
						elements[i].name,
						0,
						(DXGI_FORMAT)elements[i].type,
						elements[i].slot,
						D3D11_APPEND_ALIGNED_ELEMENT,
						(D3D11_INPUT_CLASSIFICATION)elements[i].inputClassification,
						elements[i].instanceStepRate
					};
				}
				index++;
			}

#ifdef _DEBUG
			std::stringstream ss;
			ss << "ARRAY:\n";

			for (int i = 0; i < index; i++)
			{
				ss << "Name: " << ied[i].SemanticName << "\n";
				ss << "SemanticIndex: " << ied[i].SemanticIndex << "\n";
				ss << "Slot: " << ied[i].InputSlot << "\n";
				ss << "Format: " << ied[i].Format << "\n";
				ss << "Aligned: " << ied[i].AlignedByteOffset << "\n";
				ss << "Classification: " << ied[i].InputSlotClass << "\n";
				ss << "InstanceStepRate: " << ied[i].InstanceDataStepRate << "\n";
				ss << "-----------------------\n";
			}

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)index, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_inputLayout)),
				"Failed to create input layout!\n" + vertexShaderPath + "\n" + ss.str()
			);
#else
			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)index, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_inputLayout)),
				"Failed to create input layout!\n" + vertexShaderPath
			);
#endif // DEBUG

			delete[] ied;
		}
		Shader(const Shader& other) = default;


		void Bind();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	};
}