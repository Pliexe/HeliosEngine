#include "D3D11Shader.h"

#ifdef HELIOS_PLATFORM_WINDOWS

#include <d3dcompiler.h>

namespace Helios
{

	static DXGI_FORMAT GetNativeFormat(ShaderDataType type)
	{
		switch(type)
		{
			// 8-bit
			case ShaderDataType::Bool: return DXGI_FORMAT_R8_UINT;
			// 8-bit (1 byte)
			case ShaderDataType::Int8: return DXGI_FORMAT_R8_SINT;
			case ShaderDataType::UInt8: return DXGI_FORMAT_R8_UINT;
			case ShaderDataType::NormInt8: return DXGI_FORMAT_R8_SNORM;
			case ShaderDataType::NormUInt8: return DXGI_FORMAT_R8_UNORM;
			// 16-bit (2 bytes)
			case ShaderDataType::Int16: return DXGI_FORMAT_R16_SINT;
			case ShaderDataType::Float16: return DXGI_FORMAT_R16_FLOAT;
			case ShaderDataType::UInt16: return DXGI_FORMAT_R16_UINT;
			case ShaderDataType::NormInt16: return DXGI_FORMAT_R16_SNORM;
			case ShaderDataType::NormUInt16: return DXGI_FORMAT_R16_UNORM;
			// 32-bit (4 bytes)
			case ShaderDataType::Int32: return DXGI_FORMAT_R32_SINT;
			case ShaderDataType::Float32: return DXGI_FORMAT_R32_FLOAT;
			case ShaderDataType::UInt32: return DXGI_FORMAT_R32_UINT;
			// 8-bit x 2 (2 bytes) 1 * 2 = 2
			case ShaderDataType::Int8_2: return DXGI_FORMAT_R8G8_SINT;
			case ShaderDataType::UInt8_2: return DXGI_FORMAT_R8G8_UINT;
			case ShaderDataType::NormInt8_2: return DXGI_FORMAT_R8G8_SNORM;
			case ShaderDataType::NormUInt8_2: return DXGI_FORMAT_R8G8_UNORM;
			// 16-bit x 2 (4 bytes) 2 * 2 = 4
			case ShaderDataType::Int16_2: return DXGI_FORMAT_R16G16_SINT;
			case ShaderDataType::UInt16_2: return DXGI_FORMAT_R16G16_UINT;
			case ShaderDataType::Float16_2: return DXGI_FORMAT_R16G16_FLOAT;
			case ShaderDataType::NormInt16_2: return DXGI_FORMAT_R16G16_SNORM;
			case ShaderDataType::NormUInt16_2: return DXGI_FORMAT_R16G16_UNORM;
			// 32-bit x 2 (8 bytes) 4 * 2 = 8
			case ShaderDataType::Int32_2: return DXGI_FORMAT_R32G32_SINT;
			case ShaderDataType::UInt32_2: return DXGI_FORMAT_R32G32_UINT;
			case ShaderDataType::Float32_2: return DXGI_FORMAT_R32G32_FLOAT;
			// 32-bit x 3 (12 bytes) 4 * 3 = 12
			case ShaderDataType::Int32_3: return DXGI_FORMAT_R32G32B32_SINT;
			case ShaderDataType::UInt32_3: return DXGI_FORMAT_R32G32B32_UINT;
			case ShaderDataType::Float32_3: return DXGI_FORMAT_R32G32B32_FLOAT;
			// 8-bit x 4 (4 bytes) 1 * 4 = 4
			case ShaderDataType::Int8_4: return DXGI_FORMAT_R8G8B8A8_SINT;
			case ShaderDataType::UInt8_4: return DXGI_FORMAT_R8G8B8A8_UINT;
			case ShaderDataType::NormInt8_4: return DXGI_FORMAT_R8G8B8A8_SNORM;
			case ShaderDataType::NormUInt8_4: return DXGI_FORMAT_R8G8B8A8_UNORM;
			// 16-bit x 4 (8 bytes) 2 * 4 = 8
			case ShaderDataType::Int16_4: return DXGI_FORMAT_R16G16B16A16_SINT;
			case ShaderDataType::UInt16_4: return DXGI_FORMAT_R16G16B16A16_UINT;
			case ShaderDataType::Float16_4: return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case ShaderDataType::NormInt16_4: return DXGI_FORMAT_R16G16B16A16_SNORM;
			case ShaderDataType::NormUInt16_4: return DXGI_FORMAT_R16G16B16A16_UNORM;
			// 32-bit x 4 (16 bytes) 4 * 4 = 16
			case ShaderDataType::Int32_4: return DXGI_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::UInt32_4: return DXGI_FORMAT_R32G32B32A32_UINT;
			case ShaderDataType::Float32_4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			// Matrix 2x2 (32-bit float) (16 bytes)
			case ShaderDataType::MatrixFloat2x2: return DXGI_FORMAT_R32G32_FLOAT;
			// Matrix 3x3 (32-bit float) (36 bytes)
			case ShaderDataType::MatrixFloat3x3: return DXGI_FORMAT_R32G32B32_FLOAT;
			// Matrix 4x4 (32-bit float) (64 bytes)
			case ShaderDataType::MatrixFloat4x4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		return DXGI_FORMAT_UNKNOWN;
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

	static void CreateD3D11InputLayouts(const std::string& vertex_path, ID3DBlob* blob, ID3D11InputLayout** inputLayoutPtr, InputLayouts& inputLayouts)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;

		uint32_t slot = 0;
		for (auto& inputLayout : inputLayouts)
		{
			for (auto& elem : inputLayout)
			{
				switch (elem.type)
				{
				case ShaderDataType::MatrixFloat2x2:
				case ShaderDataType::MatrixFloat3x3:
				case ShaderDataType::MatrixFloat4x4:
					int n;

					switch (elem.type)
					{
					case ShaderDataType::MatrixFloat2x2: n = 2; break;
					case ShaderDataType::MatrixFloat3x3: n = 3; break;
					case ShaderDataType::MatrixFloat4x4: n = 4; break;
					}

					for (uint32_t i = 0; i < n; i++)
					{
						ied.push_back(D3D11_INPUT_ELEMENT_DESC{
							elem.name,
							i,
							GetNativeFormat(elem.type),
							slot,
							D3D11_APPEND_ALIGNED_ELEMENT,
							slot > 0 ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
							slot > 0 ? 1u : 0u
							});
					}
					break;
				default:
					ied.push_back(D3D11_INPUT_ELEMENT_DESC{
						elem.name,
						0,
						GetNativeFormat(elem.type),
						slot,
						D3D11_APPEND_ALIGNED_ELEMENT,
						slot > 0 ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
						slot > 0 ? 1u : 0u
						});
					break;
				}
			}
			slot++;
		}

#ifdef _DEBUG
		std::stringstream ss;
		ss << "ARRAY:\n";

		for (int i = 0; i < ied.size(); i++)
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

		HRESULT hr;

		HL_EXCEPTION_HR(
			FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateInputLayout(ied.data(), (UINT)ied.size(), blob->GetBufferPointer(), blob->GetBufferSize(), inputLayoutPtr)),
			"Failed to create input layout!\n" + vertex_path + "\n" + ss.str(), hr
		);
#else
		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateInputLayout(ied.data(), (UINT)ied.size(), blob->GetBufferPointer(), blob->GetBufferSize(), inputLayoutPtr)),
			"Failed to create input layout!\n" + vertex_path
		);
#endif // DEBUG
	}

	static void CreateD3D11DepthStencil(const std::string& vertex_path, DepthFunc depthFunc, ID3D11DepthStencilState** depthStencilPtr)
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = GetDepthFuncNative(depthFunc);

		HRESULT hr;

		HL_EXCEPTION_HR(
			FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateDepthStencilState(&desc, depthStencilPtr)),
			"Failed to create depth stencil state!\n" + vertex_path, hr
		);
	}

	enum class ShaderType
	{
		VertexShader,
		PixelShader,
		GeometryShader,
	};

	template<typename ShaderType>
	constexpr auto enumToString(ShaderType value) {
		constexpr std::string_view names[] = {
			"VertexShader",
			"PixelShader",
			"GeometryShader"
		};

		return names[static_cast<std::underlying_type_t<ShaderType>>(value)];
	}

	static HRESULT CompileShader(const std::string& path, const std::string& entryPoint, const std::string& target, ID3DBlob** shaderBlob)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		// DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
		#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		#endif

		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(
			std::wstring(path.begin(), path.end()).c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(),
			target.c_str(),
			shaderFlags,
			0,
			shaderBlob,
			&errorBlob
		);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA(static_cast<const char*>(errorBlob->GetBufferPointer()));
				MessageBoxA(nullptr, static_cast<const char*>(errorBlob->GetBufferPointer()), "Shader Error", MB_OK);
				errorBlob->Release();
			}
			return hr;
		}

		if (errorBlob)
		{
			errorBlob->Release();
		}

		return S_OK;
	}

	static void D3D11FileToBlob(const std::string& path, ID3DBlob** blob, ShaderType type)
	{		
		// Check if file exists
		if (!std::filesystem::exists(path)) {
			HL_EXCEPTION(
				true, "Shader file not found!\n" + path
			);
			return;
		}

		// Check if the shader file is compiled or not (ends with .hlsl or .cso)
		if (path.ends_with(".cso")) {
			HL_EXCEPTION(
				FAILED(D3DReadFileToBlob(std::wstring(path.begin(), path.end()).c_str(), blob)),
				std::string("Failed to create blob!\nType: ") + enumToString(type).data() + "\nPath: " + path
			);
		}
		else if (path.ends_with(".hlsl")) {
			std::string profile;
			switch (type)
			{
			case ShaderType::VertexShader: profile = "vs_5_0"; break;
			case ShaderType::PixelShader: profile = "ps_5_0"; break;
			case ShaderType::GeometryShader: profile = "gs_5_0"; break;
			}

			HRESULT hr;
			HL_EXCEPTION_HR(
				FAILED(hr = CompileShader(path, "main", profile, blob)),
				std::string("Failed to create blob!\nType: ") + enumToString(type).data() + "\nPath: " + path, hr
			);
		} else {
			HL_EXCEPTION(
				true, "Shader file type not supported! (Only .cso and .hlsl are supported)\\n" + path
			);
		}
	}

	// enumToString(type).data()

	D3D11Shader::D3D11Shader(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology) : m_Name(name), m_InputLayouts(inputLayouts), m_DepthFunc(depthFunc), m_Topology(topology)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;

		D3D11FileToBlob(vertex_path, &vertexBlob, ShaderType::VertexShader);

		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &m_VertexShader)),
			"Failed to create vertex shader!\n" + vertex_path
		);

		CreateD3D11InputLayouts(vertex_path, vertexBlob.Get(), &m_InputLayout, m_InputLayouts);
        CreateD3D11DepthStencil(vertex_path, depthFunc, &m_depthStencilState);
	}
	D3D11Shader::D3D11Shader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology) : m_Name(name), m_InputLayouts(inputLayouts), m_DepthFunc(depthFunc), m_Topology(topology)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelBlob;

		D3D11FileToBlob(vertex_path, &vertexBlob, ShaderType::VertexShader);
		D3D11FileToBlob(pixel_path, &pixelBlob, ShaderType::PixelShader);

		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &m_VertexShader)),
			"Failed to create vertex shader!\n" + vertex_path
		);

		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &m_PixelShader)),
			"Failed to create pixel shader!\n" + pixel_path
		)

		CreateD3D11InputLayouts(vertex_path, vertexBlob.Get(), &m_InputLayout, m_InputLayouts);
		CreateD3D11DepthStencil(vertex_path, depthFunc, &m_depthStencilState);
	}
	D3D11Shader::D3D11Shader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology) : m_Name(name), m_InputLayouts(inputLayouts), m_DepthFunc(depthFunc), m_Topology(topology)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> geometryBlob;

		D3D11FileToBlob(vertex_path, &vertexBlob, ShaderType::VertexShader);
		D3D11FileToBlob(pixel_path, &pixelBlob, ShaderType::PixelShader);
		D3D11FileToBlob(geometry_path, &geometryBlob, ShaderType::GeometryShader);

		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &m_VertexShader)),
			"Failed to create vertex shader!\n" + vertex_path
		);

		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &m_PixelShader)),
			"Failed to create pixel shader!\n" + pixel_path
		)

		HL_EXCEPTION(
			FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateGeometryShader(geometryBlob->GetBufferPointer(), geometryBlob->GetBufferSize(), nullptr, &m_GeometryShader)),
			"Failed to create geometry shader!\n" + geometry_path
		)

		CreateD3D11InputLayouts(vertex_path, vertexBlob.Get(), &m_InputLayout, m_InputLayouts);
		CreateD3D11DepthStencil(vertex_path, depthFunc, &m_depthStencilState);
	}
	const std::string& D3D11Shader::GetName() const
	{
        return m_Name;
	}
	const DepthFunc D3D11Shader::GetDepthFunc() const
	{
        return m_DepthFunc;
	}
	const Topology D3D11Shader::GetTopology() const
	{
        return m_Topology;
	}
	void D3D11Shader::SetDepthFunc(DepthFunc depthFunc)
	{
        m_DepthFunc = depthFunc;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		m_depthStencilState->GetDesc(&depthStencilDesc);
		depthStencilDesc.DepthFunc = GetDepthFuncNative(depthFunc);

		HRESULT hr;

		HL_EXCEPTION_HR(
			FAILED (hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)),
			"Failed to create depth stencil state!", hr
		);	
	}
	void D3D11Shader::SetTopology(Topology topology)
	{
        m_Topology = topology;
	}

	static D3D11_PRIMITIVE_TOPOLOGY GetNativeTopology(Topology topology)
	{
		switch (topology)
		{
		case Topology::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Topology::LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case Topology::PointList: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		};
	}

	void D3D11Shader::Bind() const
	{
		const auto& context = Direct3D11Context::GetCurrentContext()->GetContext();

		context->VSSetShader(m_VertexShader.Get(), nullptr, 0u);
		if (m_PixelShader)
			context->PSSetShader(m_PixelShader.Get(), nullptr, 0u);
		if (m_GeometryShader)
			context->GSSetShader(m_GeometryShader.Get(), nullptr, 0u);

		context->IASetPrimitiveTopology(GetNativeTopology(m_Topology));
		context->IASetInputLayout(m_InputLayout.Get());
		context->OMSetDepthStencilState(m_depthStencilState.Get(), 1u);
	}
	void D3D11Shader::Unbind() const
	{
		const auto& context = Direct3D11Context::GetCurrentContext()->GetContext();

		context->VSSetShader(nullptr, nullptr, 0u);
		if (m_PixelShader)
			context->PSSetShader(nullptr, nullptr, 0u);
		if (m_GeometryShader)
			context->GSSetShader(nullptr, nullptr, 0u);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(nullptr);
		context->OMSetDepthStencilState(nullptr, 1u);
	}
	const Helios::InputLayouts& D3D11Shader::GetInputLayouts() const
	{
        return m_InputLayouts;
	}
}

#endif