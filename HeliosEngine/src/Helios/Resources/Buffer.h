#pragma once

#include "pch.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios {
	
	enum class BufferUsage
	{
		Default,
		Static,
		Dynamic
	};

	class ConstantBuffer
	{
	public:

		ConstantBuffer() = default;

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<ConstantBuffer> Create(uint32_t size);
		static Ref<ConstantBuffer> Create(const void* data, uint32_t size);
	};
	
	
	class VertexBuffer
	{
	protected:
		BufferUsage m_Usage;
		uint32_t m_Stride = 0u;
	public:

		virtual void Bind(uint32_t slot = 0u) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;
		
		static Ref<VertexBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Default);
		static Ref<VertexBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Default);

		template <typename T>
		void SetStride() { m_Stride = sizeof(T); }

	};

	class IndexBuffer
	{

	public:

		void Bind() const
		{
			Graphics::instance->m_deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
		}

		static Ref<IndexBuffer> Create(const unsigned short* data, uint32_t count)
		{
			D3D11_BUFFER_DESC bd = { };
			IndexBuffer indexBuffer;

			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = sizeof(unsigned short) * count;
			bd.StructureByteStride = sizeof(unsigned short);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = data;

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &indexBuffer.m_indexBuffer)),
				"Failed to create Vertex Buffer!"
			);

			indexBuffer.m_Count = count;

			return CreateRef<IndexBuffer>(std::move(indexBuffer));
		}

		inline uint32_t GetCount() const { return m_Count; }

	private:
		IndexBuffer() = default;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		uint32_t m_Count;
	};

}