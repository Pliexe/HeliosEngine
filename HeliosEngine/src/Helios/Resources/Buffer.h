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

	template <class T>
	class ConstantBuffer
	{
	public:
		//static_assert(std::is_trivially_copyable<T>::value, "ConstantBuffer<T> must be trivially copyable!");
		static_assert(std::is_class<T>::value, "ConstantBuffer<T> must be a struct!");

		// Bind the buffer to the pixel shader stage
		virtual void HELIOS_API BindPS(uint32_t slot = 0) = 0;
		// Bind the buffer to the vertex shader stage
		virtual void HELIOS_API BindVS(uint32_t slot = 0) = 0;
		virtual void HELIOS_API UnbindPS() const = 0;
		virtual void HELIOS_API UnbindVS() const = 0;

		virtual void HELIOS_API SetData(T data) = 0;
		//virtual void SetData(T* data) = 0;

		/*static Ref<ConstantBuffer> Create();
		static Ref<ConstantBuffer> Create(T data);*/

		static Ref<ConstantBuffer> Create();
		static Ref<ConstantBuffer> Create(T data);

		/*static Ref<ConstantBuffer> Create(uint32_t size, BufferUsage usage);
		static Ref<ConstantBuffer> Create(const void* data, uint32_t size, BufferUsage usage);*/
	};


	class HELIOS_API VertexBuffer
	{
	protected:
		BufferUsage m_Usage;
		uint32_t m_Stride = 0u;
		uint32_t m_Size = 0u;
	public:

		virtual void Bind(uint32_t slot = 0u) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		template <typename T>
		static Ref<VertexBuffer> Create(std::initializer_list<T> data, BufferUsage usage = BufferUsage::Default)
		{
			auto x = Create(data.begin(), data.size(), usage);
			x->m_Stride = sizeof(T);
			return x;
		}
		static Ref<VertexBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Default);
		static Ref<VertexBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Default);

		template <typename T>
		void SetStride() { m_Stride = sizeof(T); }

		template <typename T>
		inline uint32_t getCount() const { return m_Size / sizeof(T); }

	};

	class HELIOS_API IndexBuffer
	{

	public:

		void Bind() const
		{
			Graphics::instance->m_deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
		}

		static Ref<IndexBuffer> Create(std::initializer_list<uint32_t> data)
		{
			return Create((uint32_t*)data.begin(), data.size());
		}
		static Ref<IndexBuffer> Create(uint32_t* data, uint32_t count)
		{
			D3D11_BUFFER_DESC bd = { };
			IndexBuffer indexBuffer;

			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = sizeof(uint32_t) * count;
			bd.StructureByteStride = sizeof(uint32_t);
			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = data;

			HL_EXCEPTION(
				FAILED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &indexBuffer.m_indexBuffer)),
				"Failed to create Vertex Buffer!"
			);

			indexBuffer.m_Count = count;
			indexBuffer.m_Data = new uint32_t[count];
			memcpy(indexBuffer.m_Data, data, sizeof(uint32_t) * count);

			indexBuffer.m_DataStr = indexBuffer.toString();

			return CreateRef<IndexBuffer>(std::move(indexBuffer));
		}

		std::string toString() const
		{
			std::string str = std::to_string(m_Data[0]);
			for (uint32_t i = 1; i < m_Count; i++)
			{
				str += (i % 3 == 0 ? "\n" : ", ") + std::to_string(m_Data[i]);
			}
			return str;
		}


		~IndexBuffer()
		{
			delete m_Data;
		}

		inline uint32_t* data() { return m_Data; }

		inline uint32_t count() const { return m_Count; }

		std::string m_DataStr;
	private:
		IndexBuffer() = default;
		uint32_t* m_Data = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
		uint32_t m_Count;
	};
	
}
//#include "Platform/DirectX/Test.h"
//#include "Platform/DirectX/DirectXConstantBuffer.h"
//namespace Helios {
//	template <class T>
//	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create()
//	{
//		ConstantBuffer<T>* buffer = new Helios::DirectXConstantBuffer<T>(sizeof(T) + (16 - sizeof(T) % 16));
//		Ref<ConstantBuffer<T>> test = CreateRef<Test1<T>>(sizeof(T) + (16 - sizeof(T) % 16));
//		return CreateRef<Helios::DirectXConstantBuffer<T>>(sizeof(T) + (16 - sizeof(T) % 16));
//		switch (Graphics::GetAPI())
//		{
//			case Graphics::API::DirectX: return reinterpret_cast<Ref<ConstantBuffer<T>>>(CreateRe?f<Test1<T>>(sizeof(T) + (16 - sizeof(T) % 16)));
//			//case Graphics::API::DirectX: return reinterpret_cast<Ref<ConstantBuffer<T>>>(CreateRef<Test1<T>>(sizeof(T) + (16 - sizeof(T) % 16)));
//		}
//
//		HL_CORE_ASSERT(false, "Unknown Graphics API!");
//		return nullptr;
//	}
//
//	template <class T>
//	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create(T data)
//	{
//
//		switch (Graphics::GetAPI())
//		{
//		case Graphics::API::DirectX: return CreateRef<Test1<T>>(data, sizeof(T) + (16 - sizeof(T) % 16));
//		}
//
//		HL_CORE_ASSERT(false, "Unknown GraphicsAPI!");
//		return nullptr;
//	}
//
//}

#include "Buffer.inl"