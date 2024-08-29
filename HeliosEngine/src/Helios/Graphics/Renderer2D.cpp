#include "Renderer2D.h"

#include "Helios/Core/Asserts.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Helios/Core/Time.h"

#include "Helios/Resources/Shader.h"
#include "Helios/Resources/ShaderBuilder.h"

#include "Helios/Resources/Buffer.h"

#include "Helios/Math/Matrix.h"

#include "Framebuffer.h"

#include "Helios/Resources/Mesh.h"
#include "Helios/Core/Profiler.h"

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
			Matrix4x4 transform;
			Color color;
			uint32_t textureIndex;
			uint32_t entityId;
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
		Ref<VertexArray> quadVertexArray;
		Ref<IndexBuffer> quadIndexBuffer;


		Ref<UniformBuffer<TransformData>> viewProjBuffer;
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
		InputLayouts quadInputLayout = {
			{
				{ "Position", ShaderDataType::Float32_2 },
				{ "TexCoord", ShaderDataType::Float32_2 },
			},
			{
				{ "World", ShaderDataType::MatrixFloat4x4 },
				{ "Color", ShaderDataType::Float32_4 },
				{ "TextureID", ShaderDataType::UInt32 },
				{ "EntityId", ShaderDataType::UInt32 },
			}
		};

		ShaderBuilder builder;
		builder.SetName("SpriteShader");
		builder.SetVertexShader("CompiledShaders/SpriteVertexShader");
		builder.SetPixelShader("CompiledShaders/SpritePixelShader");
		builder.SetInputLayouts(quadInputLayout);

		s_Data.quadShader = builder.Create();

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

		uint32_t indices[] = {
			0, 1, 2,
			0, 2, 3
		};

		auto bs = std::array<VertexArray::BufferSpecification, 2> ();

		s_Data.quadVertexArray = VertexArray::Create(quadInputLayout, {
			{ vertices, sizeof(vertices) },
			{ nullptr, sizeof(Renderer2DData::QuadInstanceData) * Renderer2DData::MaxQuads, BufferUsage::Dynamic }
		});
		s_Data.quadIndexBuffer = IndexBuffer::Create(indices, std::size(indices));
		s_Data.quadVertexArray->SetIndexBuffer(s_Data.quadIndexBuffer);

		s_Data.viewProjBuffer = UniformBuffer<Renderer2DData::TransformData>::Create(0u);
		//assert(s_Data.viewProjBuffer == nullptr);

		D3D11_SAMPLER_DESC samplerDesc = {};
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		
		Direct3D11Context::GetCurrentContext()->GetDevice()->CreateSamplerState(&samplerDesc, &sampler);

		return true;
	}

	void Renderer2D::Shutdown()
	{
		
	}
	
	void Renderer2D::BeginScene(Matrix4x4 projection)
	{
		Renderer2DData::TransformData data = { projection };

		s_Data.viewProjBuffer->SetData(data);
	}
	
	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		HL_PROFILE_BEGIN("Renderer2D Flush");
		if ((s_Data.quadInstanceDataPtr - s_Data.quadInstanceData) > 0)
		{
			s_Data.quadVertexArray->GetVertexBuffers()[1]->SetData(s_Data.quadInstanceData, (s_Data.quadInstanceDataPtr - s_Data.quadInstanceData) * sizeof(Renderer2DData::QuadInstanceData));
			s_Data.quadShader->Bind();
			//s_Data.quadIndexBuffer->Bind();
			s_Data.viewProjBuffer->Bind();
			s_Data.quadVertexArray->Bind();

			for (uint8_t i = 0; i < s_Data.textureSlotIndex; i++)
			{
				s_Data.textureSlots[i]->Bind(i);
			}
			
			Direct3D11Context::GetCurrentContext()->GetContext()->PSSetSamplers(0u, 1u, sampler.GetAddressOf());
			
			HL_PROFILE_BEGIN("Renderer2D Wait for GPU");
			Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexedInstanced(6u, (s_Data.quadInstanceDataPtr - s_Data.quadInstanceData), 0u, 0u, 0u);
			HL_PROFILE_END();
			s_Data.quadInstanceDataPtr = s_Data.quadInstanceData;
			s_Data.textureSlotIndex = 0;
		}
		HL_PROFILE_END();
	}

	void Renderer2D::DrawSprite(uint32_t entityId, Matrix4x4 worldMatrix, SpriteRendererComponent sprite)
	{
		if ((s_Data.quadInstanceDataPtr - s_Data.quadInstanceData) > Renderer2DData::MaxQuads)
			Flush();

		if(sprite.texture == nullptr)
		{
			*s_Data.quadInstanceDataPtr = {
				worldMatrix,
				sprite.color,
				128u,
				entityId
			};
		}
		else
		{
			if(s_Data.textureSlotIndex >= Renderer2DData::MaxTextureSlots)
				Flush();

			s_Data.textureSlots[s_Data.textureSlotIndex] = sprite.texture;
			*s_Data.quadInstanceDataPtr = {
				worldMatrix,
				{ sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a },
				s_Data.textureSlotIndex,
				entityId
			};
			s_Data.textureSlotIndex++;
		}

		/*s_Data.quadInstanceDataPtr->transform = (Matrix4x4::Scale(transform.scale) * Matrix4x4::RotationColumn(transform.rotation) * Matrix4x4::TranslationRow(transform.position)).matrix*/;
		//s_Data.quadInstanceDataPtr->color = sprite.color.c;
		s_Data.quadInstanceDataPtr++;
	}

	// void Renderer2D::DrawCube(TransformComponent transform, SpriteRendererComponent sprite)
	// {

	// 	cubeMesh->BindVS();
	// 	cubeShader->BindVS();

		
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
	// 				Matrix4x4::RotationRow(transform.rotation) *
	// 				Matrix4x4::TranslationRow(transform.position) *
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
	// 	//Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer)), "Failed to create constant buffer!\n" + GetLastErrorAsString());
	
	// 	// bind
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed((UINT)cubeMesh->getIndexCount(), 0u, 0u);
	// 	HL_ASSERT(SUCCEEDED(GetLastError()), GetLastErrorAsString());
	// }
	
	// void Renderer2D::DrawSprite(TransformComponent transform, SpriteRendererComponent sprite)
	// {
	// 	Renderer2DData::quadVertexBuffer->BindVS();
	// 	Renderer2DData::quadIndexBuffer->BindVS();

	// 	Renderer2DData::quadShader->BindVS();
		
		

	// 	auto euler = transform.rotation.eulerRads();

	// 	const ConstantBuffer cb =
	// 	{
	// 		{
	// 			Matrix4x4::Transpose(
	// 				Matrix4x4::Scale(transform.scale) *
	// 				Matrix4x4::RotationRow(transform.rotation) *
	// 				Matrix4x4::TranslationRow(transform.position) *
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
	// 	//Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&cbd, &csd, &pConstantBuffer)), "Failed to create constant buffer!\n" + GetLastErrorAsString());
	
	// 	// bind
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// 	/*spriteShader.reset();
	// 	spriteShader = CreateRef<Shader>(Shader(std::string("Sprite"), {
	// 		{ "Position", Shader::DataType::Float2 },
	// 		{ "Color", Shader::DataType::Float4 }
	// 	}));

	// 	spriteShader->BindVS();*/


	// 	//Direct3D11Context::GetCurrentContext()->GetContext()->OMSetRenderTargets(1u, s_renderTarget.GetAddressOf(), nullptr);


	// 	/*D3D11_VIEWPORT vp;
	// 	vp.Width = 300;
	// 	vp.Height = 300;
	// 	vp.MinDepth = 0;
	// 	vp.MaxDepth = 1;
	// 	vp.TopLeftX = 0;
	// 	vp.TopLeftY = 0;
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->RSSetViewports(1u, &vp);*/


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed((UINT)std::Size(indices), 0u, 0u);
	// 	HL_ASSERT(SUCCEEDED(GetLastError()), GetLastErrorAsString());
	// }


	// void Renderer2D::DrawPolygon(int sides)
	// {
	// 	struct MeshVertex
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
	// 	MeshVertex * vertices = new MeshVertex[sides + 2];

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
	// 	bd.ByteWidth = sizeof(MeshVertex)*(sides + 2);
	// 	bd.StructureByteStride = sizeof(MeshVertex);
	// 	D3D11_SUBRESOURCE_DATA sd = {};
	// 	sd.pSysMem = vertices;
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer)), "Failed to create buffer");

	// 	// bind vertex buffer
	// 	const UINT stride = sizeof(MeshVertex);
	// 	const UINT offset = 0u;
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


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
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bdi, &sdi, &pIndexBuffer)), "Failed to create buffer");

	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	// 	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	// 	HL_ASSERT(SUCCEEDED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)), "Failed to read Pixel Shader!");
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)), "Failed to create MeshVertex Shader!");

	// 	Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	// 	HL_ASSERT(SUCCEEDED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)), "Failed to read MeshVertex Shader!");
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)), "Failed to create MeshVertex Shader!");


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	// 	const D3D11_INPUT_ELEMENT_DESC ied[] = {
	// 		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	// 	};
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateInputLayout(ied, (UINT)std::Size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)), "Failed to create input layout!");


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetInputLayout(pInputLayout.Get());


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 	//Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// 	/*D3D11_VIEWPORT vp;
	// 	vp.Width = 300;
	// 	vp.Height = 300;
	// 	vp.MinDepth = 0;
	// 	vp.MaxDepth = 1;
	// 	vp.TopLeftX = 0;
	// 	vp.TopLeftY = 0;
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->RSSetViewports(1u, &vp);*/


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->DrawIndexed((UINT)(sides * 3 + 3), 0u, 0u);
	// 	HL_ASSERT(SUCCEEDED(GetLastError()), GetLastErrorAsString());
		
	// 	delete[] vertices;
	// 	delete[] indecies;
	// }
	
	// void Renderer2D::DrawTriangle(Vector2 position)
	// {
	// 	struct MeshVertex
	// 	{
	// 		float x;
	// 		float y;
	// 		float r;
	// 		float g;
	// 		float b;
	// 	};
		

	// 	const MeshVertex vertices[] = {
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
	// 	bd.StructureByteStride = sizeof(MeshVertex);
	// 	D3D11_SUBRESOURCE_DATA sd = {};
	// 	sd.pSysMem = vertices;
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer)), "Failed to create buffer");
		
	// 	// bind vertex buffer
	// 	const UINT stride = sizeof(MeshVertex);
	// 	const UINT offset = 0u;
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	// 	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	// 	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	// 	HL_ASSERT(SUCCEEDED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)), "Failed to read Pixel Shader!");
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)), "Failed to create MeshVertex Shader!");

	// 	Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
		
	// 	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	// 	HL_ASSERT(SUCCEEDED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)), "Failed to read MeshVertex Shader!");
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)), "Failed to create MeshVertex Shader!");


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// 	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	// 	const D3D11_INPUT_ELEMENT_DESC ied[] = {
	// 		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 		{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	// 	};
	// 	HL_ASSERT(SUCCEEDED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateInputLayout(ied, (UINT)std::Size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)), "Failed to create input layout!");
		

	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetInputLayout(pInputLayout.Get());

	// 	Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 	//Direct3D11Context::GetCurrentContext()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// 	D3D11_VIEWPORT vp;
	// 	vp.Width = 300;
	// 	vp.Height = 300;
	// 	vp.MinDepth = 0;
	// 	vp.MaxDepth = 1;
	// 	vp.TopLeftX = 0;
	// 	vp.TopLeftY = 0;
	// 	Direct3D11Context::GetCurrentContext()->GetContext()->RSSetViewports(1u, &vp);


	// 	Direct3D11Context::GetCurrentContext()->GetContext()->Draw((UINT)std::Size(vertices), 0u);
	// 	HL_ASSERT(SUCCEEDED(GetLastError()), GetLastErrorAsString());		
	// }
}
