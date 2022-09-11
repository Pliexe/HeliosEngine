#pragma once

#include "pch.h"
#include "Helios/Core/Asserts.h"

namespace Helios {
	
	class VertexBuffer
	{
	public:
		void Bind() const
		{
			Graphics::instance->m_deviceContext->IASetVertexBuffers(0u, 1u, m_Data.GetAddressOf(), &m_Stride, &m_Offset);
		}

		template <typename T>
		static Ref<VertexBuffer> Create(const T* data, uint32_t size)
		{
			D3D11_BUFFER_DESC bd = { };
			VertexBuffer vertexBuffer;
			vertexBuffer.m_Stride = sizeof(T);

			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = sizeof(T) * size;
			bd.StructureByteStride = sizeof(T);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = data;

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &vertexBuffer.m_Data)),
				"Failed to create Vertex Buffer!"
			);

			return CreateRef<VertexBuffer>(std::move(vertexBuffer));
		}

	private:
		VertexBuffer() = default;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Data;
		UINT m_Stride;
		UINT m_Offset = 0u;
	};

	class IndexBuffer
	{

	public:

		void Bind() const
		{
			Graphics::instance->m_deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
		}

		static Ref<IndexBuffer> Create(const unsigned short* data, uint32_t size)
		{
			D3D11_BUFFER_DESC bd = { };
			IndexBuffer indexBuffer;

			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = sizeof(unsigned short) * size;
			bd.StructureByteStride = sizeof(unsigned short);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = data;

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &indexBuffer.m_indexBuffer)),
				"Failed to create Vertex Buffer!"
			);

			return CreateRef<IndexBuffer>(std::move(indexBuffer));
		}

	private:
		IndexBuffer() = default;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		UINT m_count;
	};

}