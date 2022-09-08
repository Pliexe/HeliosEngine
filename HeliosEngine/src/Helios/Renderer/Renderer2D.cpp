#include "Renderer2D.h"

#include "Helios/Graphics/Graphics.h"
#include "Helios/Core/Asserts.h"
#include <d3dcompiler.h>

#include "Helios/Core/Time.h"

namespace Helios {

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> Renderer2D::s_renderTarget;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Renderer2D::s_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Renderer2D::s_renderTargetTexture;

	bool Renderer2D::CreateRenderTarget(UINT width, UINT height)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HRESULT hr;
		if (FAILED(hr = Graphics::instance->m_device->CreateTexture2D(&textureDesc, NULL, &s_renderTargetTexture))) {
			HL_CORE_ASSERT_WITH_MSG(false, "Failed to create texture for render target!\n" + GetLastErrorAsString(hr));
				return false;
		}

		// Setup the description of the render target view.
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		if (FAILED(hr = Graphics::instance->m_device->CreateRenderTargetView(s_renderTargetTexture.Get(), &renderTargetViewDesc, &s_renderTarget))) {
			HL_CORE_ASSERT_WITH_MSG(false, "Failed to create render target view!\n" + GetLastErrorAsString(hr));
			return false;
		}

		// Setup the description of the shader resource view.
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		if (FAILED(hr = Graphics::instance->m_device->CreateShaderResourceView(s_renderTargetTexture.Get(), &shaderResourceViewDesc, &s_shaderResourceView))) {
			HL_CORE_ASSERT_WITH_MSG(false, "Failed to create render target view!\n" + GetLastErrorAsString(hr));
			return false;
		}
		
		return true;
	}

	bool Renderer2D::Init()
	{
		return CreateRenderTarget(300, 300);
	}

	void Renderer2D::Shutdown()
	{
		s_shaderResourceView.Reset();
		s_renderTarget.Reset();
		s_renderTargetTexture.Reset();
	}
	void Renderer2D::BeginScene(Components::Camera& cam)
	{
		Graphics::instance->m_deviceContext->OMSetRenderTargets(1, s_renderTarget.GetAddressOf(), NULL);
		Graphics::instance->m_deviceContext->ClearRenderTargetView(s_renderTarget.Get(), cam.clear_color);
	}
	
	void Renderer2D::EndScene()
	{
		Graphics::instance->m_deviceContext->OMSetRenderTargets(1, &Graphics::instance->m_mainRenderTarget, NULL);
	}
	
	void Renderer2D::Resize(unsigned int width, unsigned int height)
	{
		// No need to Release render target since it's using ComPtr where refrencing it by & will automatically release it!
		//Graphics::instance->m_pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		Shutdown();
		CreateRenderTarget(width, height);
	}

	ImTextureID Renderer2D::GetImGuiTexture()
	{
		return (void*)s_shaderResourceView.Get();
	}

	ImVec2 Renderer2D::GetRenderTargetSize() {
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		s_renderTargetTexture->GetDesc(&desc);
		return { (float)desc.Width, (float)desc.Height };
	}

	void Renderer2D::DrawHexagon(int sides)
	{
		struct Vertex
		{
			struct {
				float x;
				float y;
			} pos;
			struct {
				float r;
				float g;
				float b;
			} col;
		};


		// cube

		const ImVec2 viewportSize = GetRenderTargetSize();


		unsigned short* indecies = new unsigned short[sides * 3 + 3 + 3];
		Vertex * vertices = new Vertex[sides + 2];

		vertices[0].pos = { 0.0f, 0.0f };
		vertices[0].col = { 0.0f, 1.0f, 1.0f };

		//vertices[0].col = { (tan((float)Time::passedTime()) + 1) / 2, (cos((float)Time::passedTime()) + 1) / 2, (sin((float)Time::passedTime()) + 1) / 2 };

		int i;
		for (i = 0; i <= sides; i++)
		{
			auto offset = i * 3;
			float radians = (i * (360.0f / (float)sides)) * 3.14f / 180;
			indecies[offset] = 0;
			indecies[offset + 1] = i + 1;
			indecies[offset + 2] = i + 2;

			vertices[i + 1].pos = { sin(radians) * (viewportSize.y / viewportSize.x), cos(radians)};
			vertices[i + 1].col = { (sin(radians + (float)Time::passedTime() * 3.5f) + 1) / 2.0f, 0.0f, 1.0f};
		}
		
		vertices[i].pos = vertices[1].pos;

		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = { };
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(Vertex)*(sides + 2);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &pVertexBuffer)), "Failed to create buffer");

		// bind vertex buffer
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		Graphics::instance->m_deviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC bdi = { };
		bdi.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bdi.Usage = D3D11_USAGE_DEFAULT;
		bdi.CPUAccessFlags = 0u;
		bdi.MiscFlags = 0u;
		bdi.ByteWidth = sizeof(unsigned short) * (sides * 3 + 3);
		bdi.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA sdi = {};
		sdi.pSysMem = indecies;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&bdi, &sdi, &pIndexBuffer)), "Failed to create buffer");

		Graphics::instance->m_deviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)), "Failed to read Pixel Shader!");
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)), "Failed to create Vertex Shader!");

		Graphics::instance->m_deviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)), "Failed to read Vertex Shader!");
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)), "Failed to create Vertex Shader!");


		Graphics::instance->m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] = {
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)), "Failed to create input layout!");


		Graphics::instance->m_deviceContext->IASetInputLayout(pInputLayout.Get());

		Graphics::instance->m_deviceContext->OMSetRenderTargets(1u, s_renderTarget.GetAddressOf(), nullptr);

		Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



		D3D11_VIEWPORT vp;
		vp.Width = viewportSize.x;
		vp.Height = viewportSize.y;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		Graphics::instance->m_deviceContext->RSSetViewports(1u, &vp);


		Graphics::instance->m_deviceContext->DrawIndexed((UINT)(sides * 3 + 3), 0u, 0u);
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(GetLastError()), GetLastErrorAsString());
		
		delete[] vertices;
		delete[] indecies;
	}
	
	void Renderer2D::DrawTriangle(Vector2D position)
	{
		struct Vertex
		{
			float x;
			float y;
			float r;
			float g;
			float b;
		};

		// cube

		const ImVec2 viewportSize = GetRenderTargetSize();
		float scaleY = (viewportSize.y / 720.0f);
		float scaleX = ((viewportSize.y - viewportSize.x + viewportSize.y)) / 720.0f;

		const Vertex vertices[] = {
			{ -0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },
			{ -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },

			{  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, -0.5f, 0.0f, 0.0f, 1.0f }
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = { };
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &pVertexBuffer)), "Failed to create buffer");
		
		// bind vertex buffer
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		Graphics::instance->m_deviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)), "Failed to read Pixel Shader!");
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader)), "Failed to create Vertex Shader!");

		Graphics::instance->m_deviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
		
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(D3DReadFileToBlob(L"VertexShader.cso", &pBlob)), "Failed to read Vertex Shader!");
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)), "Failed to create Vertex Shader!");


		Graphics::instance->m_deviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] = {
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(Graphics::instance->m_device->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout)), "Failed to create input layout!");
		

		Graphics::instance->m_deviceContext->IASetInputLayout(pInputLayout.Get());

		Graphics::instance->m_deviceContext->OMSetRenderTargets(1u, s_renderTarget.GetAddressOf(), nullptr);

		Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//Graphics::instance->m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



		D3D11_VIEWPORT vp;
		vp.Width = viewportSize.x;
		vp.Height = viewportSize.y;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		Graphics::instance->m_deviceContext->RSSetViewports(1u, &vp);


		Graphics::instance->m_deviceContext->Draw((UINT)std::size(vertices), 0u);
		HL_CORE_ASSERT_WITH_MSG(SUCCEEDED(GetLastError()), GetLastErrorAsString());		
	}
}
