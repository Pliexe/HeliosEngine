#include "Renderer2D.h"

#include "Helios/Graphics/Graphics.h"
#include "Helios/Core/Asserts.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Helios/Core/Time.h"

#include "Helios/Resources/Shader.h"

#include "Helios/Resources/Buffer.h"

#include "Helios/Translation/Matrix.h"

#include "Framebuffer.h"

#include "Helios/Resources/Mesh.h"

namespace Helios {


	struct Renderer2DData
	{
		static const uint16_t MaxQuads = 10000;
		static const uint8_t MaxTextureSlots = 128;

		struct QuadVertex
		{
			Vector2 position;
			Vector2 texCoord;
		};

		struct QuadInstanceData
		{
			uint8_t textureIndex;
			Color color;
			Matrix4x4 transform;
		};

		struct TransformData
		{
			Matrix4x4 viewProjection;
		};

		std::array<Ref<Texture2D>, MaxTextureSlots> textureSlots;
		uint8_t textureSlotIndex = 0;
		 
		QuadInstanceData* quadInstanceData = new QuadInstanceData[MaxQuads];
		QuadInstanceData* quadInstanceDataPtr = quadInstanceData;
		
		Ref<Shader> quadShader;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<IndexBuffer> quadIndexBuffer;
		Ref<VertexBuffer> quadInstanceBuffer;


		Ref<ConstantBuffer> viewProjBuffer;
	};

	Renderer2DData s_Data;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	struct Vertex
	{
		float x;
		float y;
		float r;
		float g;
		float b;
		float a;
	};

	bool Renderer2D::Init()
	{
		s_Data.quadShader = CreateRef<Shader>(Shader("Sprite", {
			{ "Position", Shader::DataType::Float2 },
			{ "TexCoord", Shader::DataType::Float2 },

			{ "TextureID", Shader::DataType::UInt8, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "Color", Shader::DataType::Float4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
			{ "World", Shader::DataType::Matrix4x4, 1u, 1u, Shader::ShaderElement::InputClassification::PerInstance },
		}));

		// const Renderer2DData::QuadVertex vertices[] = {
		// 	{ { -0.5f,  0.5f } },
		// 	{ {  0.5f,  0.5f } },
		// 	{ {  0.5f, -0.5f } },
		// 	{ { -0.5f, -0.5f } }
		// };


		const Renderer2DData::QuadVertex vertices[] = {
			{ { -0.5f,  0.5f }, { 0.0f, 0.0f } },
			{ {  0.5f,  0.5f }, { 1.0f, 0.0f } },
			{ {  0.5f, -0.5f }, { 1.0f, 1.0f } },
			{ { -0.5f, -0.5f }, { 0.0f, 1.0f } },
		};

		const unsigned short indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		s_Data.quadVertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		s_Data.quadVertexBuffer->SetStride<Renderer2DData::QuadVertex>();
		s_Data.quadIndexBuffer = IndexBuffer::Create(indices, std::size(indices));
		s_Data.quadInstanceBuffer = VertexBuffer::Create(sizeof(Renderer2DData::QuadInstanceData) * Renderer2DData::MaxQuads, BufferUsage::Dynamic);
		s_Data.quadInstanceBuffer->SetStride<Renderer2DData::QuadInstanceData>();

		s_Data.viewProjBuffer = ConstantBuffer::Create(sizeof(Renderer2DData::TransformData));

		D3D11_SAMPLER_DESC samplerDesc = {};
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		
		Graphics::instance->m_device->CreateSamplerState(&samplerDesc, &sampler);

		return true;
	}

	void Renderer2D::Shutdown()
	{
		
	}
	
	void Renderer2D::BeginScene(Matrix4x4 projection)
	{
		Renderer2DData::TransformData data = { projection };

		s_Data.viewProjBuffer->SetData(&data, sizeof(Renderer2DData::TransformData));
	}
	
	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		if ((s_Data.quadInstanceDataPtr - s_Data.quadInstanceData) > 0)
		{
			s_Data.quadInstanceBuffer->SetData(s_Data.quadInstanceData, (s_Data.quadInstanceDataPtr - s_Data.quadInstanceData) * sizeof(Renderer2DData::QuadInstanceData));
			s_Data.quadShader->Bind();
			s_Data.quadVertexBuffer->Bind();
			s_Data.quadIndexBuffer->Bind();
			s_Data.quadInstanceBuffer->Bind(1u);
			s_Data.viewProjBuffer->Bind(0u);

			for (uint8_t i = 0; i < s_Data.textureSlotIndex; i++)
			{
				s_Data.textureSlots[i]->Bind(i);
			}

			Graphics::instance->m_deviceContext->PSSetSamplers(0u, 1u, sampler.GetAddressOf());
			
			Graphics::instance->m_deviceContext->DrawIndexedInstanced(6u, (s_Data.quadInstanceDataPtr - s_Data.quadInstanceData), 0u, 0u, 0u);
			s_Data.quadInstanceDataPtr = s_Data.quadInstanceData;
			s_Data.textureSlotIndex = 0;
		}
	}

	void Renderer2D::DrawSprite(Components::Transform transform, Components::SpriteRenderer sprite)
	{
		if ((s_Data.quadInstanceDataPtr - s_Data.quadInstanceData) > Renderer2DData::MaxQuads)
			Flush();

		if(sprite.texture == nullptr)
		{
			*s_Data.quadInstanceDataPtr = {
				128u,
				{ sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a },
				{
					Matrix4x4::Transpose(
						Matrix4x4::Scale(transform.scale) *
						Matrix4x4::Rotation(transform.rotation) *
						Matrix4x4::Translation(transform.position)
					)
				}
			};
		}
		else
		{
			if(s_Data.textureSlotIndex >= Renderer2DData::MaxTextureSlots)
				Flush();

			s_Data.textureSlots[s_Data.textureSlotIndex] = sprite.texture;
			*s_Data.quadInstanceDataPtr = {
				s_Data.textureSlotIndex,
				{ sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a },
				{
					Matrix4x4::Transpose(
						Matrix4x4::Scale(transform.scale) *
						Matrix4x4::Rotation(transform.rotation) *
						Matrix4x4::Translation(transform.position)
					)
				}
			};
			s_Data.textureSlotIndex++;
		}

		/*s_Data.quadInstanceDataPtr->transform = (Matrix4x4::Scale(transform.scale) * Matrix4x4::RotationColumn(transform.rotation) * Matrix4x4::Translation(transform.position)).matrix*/;
		//s_Data.quadInstanceDataPtr->color = sprite.color.c;
		s_Data.quadInstanceDataPtr++;
	}

	// void Renderer2D::DrawCube(Components::Transform transform, Components::SpriteRenderer sprite)
	// {

	// 	cubeMesh->Bind();
	// 	cubeShader->Bind();

		
	// 	struct ConstantBuffer
	// 	{
	// 		Matrix4x4 transform;
	// 	};

	// 	auto euler = transform.rotation.eulerRads();

	// 	const ConstantBuffer cb =
	// 	{
	// 		{
	// 			Matrix4x4::Transpose(
	// 				Matrix4x4::Scale(transform.scale) *
	// 				Matrix4x4::Rotation(transform.rotation) *
	// 				Matrix4x4::Translation(transform.position) *
	// 				projectionMatrix
	// 			)
	// 		}
	// 	};

	// 	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	// 	D3D11_BUFFER_DESC cbd;
	// 	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// 	cbd.Usage = D3D11_USAGE_DYNAMIC;
	// 	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 	cbd.MiscFlags = 0u;
	// 	cbd.ByteWidth = sizeof(cb);
	// 	cbd.StructureByteStride = 0u;
	// 	D3D11_SUBRESOURCE_DATA csd = {};
	// 	csd.pSysMem = &cb;
	// 	//Graphics::instance->m_device->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&cbd, &csd, &pConstantBuffer)), "Failed to create constant buffer!\n" + GetLastErrorAsString());
	
	// 	// bind
	// 	Graphics::instance->m_deviceContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


	// 	Graphics::instance->m_deviceContext->DrawIndexed((UINT)cubeMesh->GetIndexCount(), 0u, 0u);
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(GetLastError()), GetLastErrorAsString());
	// }
	
	// void Renderer2D::DrawSprite(Components::Transform transform, Components::SpriteRenderer sprite)
	// {
	// 	Renderer2DData::quadVertexBuffer->Bind();
	// 	Renderer2DData::quadIndexBuffer->Bind();

	// 	Renderer2DData::quadShader->Bind();
		
		

	// 	auto euler = transform.rotation.eulerRads();

	// 	const ConstantBuffer cb =
	// 	{
	// 		{
	// 			Matrix4x4::Transpose(
	// 				Matrix4x4::Scale(transform.scale) *
	// 				Matrix4x4::Rotation(transform.rotation) *
	// 				Matrix4x4::Translation(transform.position) *
	// 				projectionMatrix
					
	// 			)
	// 		}
	// 	};

	// 	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	// 	D3D11_BUFFER_DESC cbd;
	// 	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// 	cbd.Usage = D3D11_USAGE_DYNAMIC;
	// 	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 	cbd.MiscFlags = 0u;
	// 	cbd.ByteWidth = sizeof(cb);
	// 	cbd.StructureByteStride = 0u;
	// 	D3D11_SUBRESOURCE_DATA csd = {};
	// 	csd.pSysMem = &cb;
	// 	//Graphics::instance->m_device->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&cbd, &csd, &pConstantBuffer)), "Failed to create constant buffer!\n" + GetLastErrorAsString());
	
	// 	// bind
	// 	Graphics::instance->m_deviceContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// 	/*spriteShader.reset();
	// 	spriteShader = CreateRef<Shader>(Shader(std::string("Sprite"), {
	// 		{ "Position", Shader::DataType::Float2 },
	// 		{ "Color", Shader::DataType::Float4 }
	// 	}));

	// 	spriteShader->Bind();*/


	// 	//Graphics::instance->m_deviceContext->OMSetRenderTargets(1u, s_renderTarget.GetAddressOf(), nullptr);


	// 	/*D3D11_VIEWPORT vp;
	// 	vp.Width = 300;
	// 	vp.Height = 300;
	// 	vp.MinDepth = 0;
	// 	vp.MaxDepth = 1;
	// 	vp.TopLeftX = 0;
	// 	vp.TopLeftY = 0;
	// 	Graphics::instance->m_deviceContext->RSSetViewports(1u, &vp);*/


	// 	Graphics::instance->m_deviceContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(GetLastError()), GetLastErrorAsString());
	// }


	// void Renderer2D::DrawPolygon(int sides)
	// {
	// 	struct Vertex
	// 	{
	// 		struct {
	// 			float x;
	// 			float y;
	// 		} pos;
	// 		struct {
	// 			float r;
	// 			float g;
	// 			float b;
	// 		} col;
	// 	};


	// 	// cube


	// 	unsigned short* indecies = new unsigned short[sides * 3 + 3 + 3];
	// 	Vertex * vertices = new Vertex[sides + 2];

	// 	vertices[0].pos = { 0.0f, 0.0f };
	// 	vertices[0].col = { 0.0f, 1.0f, 1.0f };

	// 	//vertices[0].col = { (tan((float)Time::passedTime()) + 1) / 2, (cos((float)Time::passedTime()) + 1) / 2, (sin((float)Time::passedTime()) + 1) / 2 };

	// 	int i;
	// 	for (i = 0; i <= sides; i++)
	// 	{
	// 		auto offset = i * 3;
	// 		float radians = (i * (360.0f / (float)sides)) * 3.14f / 180;
	// 		indecies[offset] = 0;
	// 		indecies[offset + 1] = i + 1;
	// 		indecies[offset + 2] = i + 2;

	// 		vertices[i + 1].pos = { sin(radians), cos(radians)};
	// 		vertices[i + 1].col = { (sin(radians + (float)Time::passedTime() * 3.5f) + 1) / 2.0f, 0.0f, 1.0f};
	// 	}
		
	// 	vertices[i].pos = vertices[1].pos;

	// 	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	// 	D3D11_BUFFER_DESC bd = { };
	// 	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// 	bd.Usage = D3D11_USAGE_DEFAULT;
	// 	bd.CPUAccessFlags = 0u;
	// 	bd.MiscFlags = 0u;
	// 	bd.ByteWidth = sizeof(Vertex)*(sides + 2);
	// 	bd.StructureByteStride = sizeof(Vertex);
	// 	D3D11_SUBRESOURCE_DATA sd = {};
	// 	sd.pSysMem = vertices;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &pVertexBuffer)), "Failed to create buffer");

	// 	// bind vertex buffer
	// 	const UINT stride = sizeof(Vertex);
	// 	const UINT offset = 0u;
	// 	Graphics::instance->m_deviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	// 	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	// 	D3D11_BUFFER_DESC bdi = { };
	// 	bdi.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// 	bdi.Usage = D3D11_USAGE_DEFAULT;
	// 	bdi.CPUAccessFlags = 0u;
	// 	bdi.MiscFlags = 0u;
	// 	bdi.ByteWidth = sizeof(unsigned short) * (sides * 3 + 3);
	// 	bdi.StructureByteStride = sizeof(unsigned short);
	// 	D3D11_SUBRESOURCE_DATA sdi = {};
	// 	sdi.pSysMem = indecies;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&bdi, &sdi, &pIndexBuffer)), "Failed to create buffer");

	// 	Graphics::instance->m_deviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	// 	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)), "Failed to read Pixel Shader!");
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)), "Failed to create Vertex Shader!");

	// 	Graphics::instance->m_deviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)), "Failed to read Vertex Shader!");
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)), "Failed to create Vertex Shader!");


	// 	Graphics::instance->m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	// 	const D3D11_INPUT_ELEMENT_DESC ied[] = {
	// 		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	// 	};
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)), "Failed to create input layout!");


	// 	Graphics::instance->m_deviceContext->IASetInputLayout(pInputLayout.Get());


	// 	Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 	//Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// 	/*D3D11_VIEWPORT vp;
	// 	vp.Width = 300;
	// 	vp.Height = 300;
	// 	vp.MinDepth = 0;
	// 	vp.MaxDepth = 1;
	// 	vp.TopLeftX = 0;
	// 	vp.TopLeftY = 0;
	// 	Graphics::instance->m_deviceContext->RSSetViewports(1u, &vp);*/


	// 	Graphics::instance->m_deviceContext->DrawIndexed((UINT)(sides * 3 + 3), 0u, 0u);
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(GetLastError()), GetLastErrorAsString());
		
	// 	delete[] vertices;
	// 	delete[] indecies;
	// }
	
	// void Renderer2D::DrawTriangle(Vector2 position)
	// {
	// 	struct Vertex
	// 	{
	// 		float x;
	// 		float y;
	// 		float r;
	// 		float g;
	// 		float b;
	// 	};
		

	// 	const Vertex vertices[] = {
	// 		{ -0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
	// 		{  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },
	// 		{ -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },

	// 		{  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },
	// 		{  0.5f, -0.5f, 0.0f, 1.0f, 1.0f },
	// 		{ -0.5f, -0.5f, 0.0f, 0.0f, 1.0f }
	// 	};
	// 	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	// 	D3D11_BUFFER_DESC bd = { };
	// 	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// 	bd.Usage = D3D11_USAGE_DEFAULT;
	// 	bd.CPUAccessFlags = 0u;
	// 	bd.MiscFlags = 0u;
	// 	bd.ByteWidth = sizeof(vertices);
	// 	bd.StructureByteStride = sizeof(Vertex);
	// 	D3D11_SUBRESOURCE_DATA sd = {};
	// 	sd.pSysMem = vertices;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &pVertexBuffer)), "Failed to create buffer");
		
	// 	// bind vertex buffer
	// 	const UINT stride = sizeof(Vertex);
	// 	const UINT offset = 0u;
	// 	Graphics::instance->m_deviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	// 	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	// 	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)), "Failed to read Pixel Shader!");
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)), "Failed to create Vertex Shader!");

	// 	Graphics::instance->m_deviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
		
	// 	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)), "Failed to read Vertex Shader!");
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)), "Failed to create Vertex Shader!");


	// 	Graphics::instance->m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	// 	const D3D11_INPUT_ELEMENT_DESC ied[] = {
	// 		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	// 	};
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)), "Failed to create input layout!");
		

	// 	Graphics::instance->m_deviceContext->IASetInputLayout(pInputLayout.Get());

	// 	Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 	//Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// 	D3D11_VIEWPORT vp;
	// 	vp.Width = 300;
	// 	vp.Height = 300;
	// 	vp.MinDepth = 0;
	// 	vp.MaxDepth = 1;
	// 	vp.TopLeftX = 0;
	// 	vp.TopLeftY = 0;
	// 	Graphics::instance->m_deviceContext->RSSetViewports(1u, &vp);


	// 	Graphics::instance->m_deviceContext->Draw((UINT)std::size(vertices), 0u);
	// 	HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(GetLastError()), GetLastErrorAsString());		
	// }
}
