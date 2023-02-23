#include "Buffer.h"
#include "Platform/DirectX/DirectXConstantBuffer.h"
#include "Platform/DirectX/DirectXVertexBuffer.h"

namespace Helios
{
	/*template <typename T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create()
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX: return CreateRef<DirectXConstantBuffer>(sizeof(T) + (16 - sizeof(T) % 16));
		}

		HL_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	template <typename T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create(T data)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX: return CreateRef<DirectXConstantBuffer>(data, sizeof(T) + (16 - sizeof(T) % 16));
		}

		HL_CORE_ASSERT(false, "Unknown GraphicsAPI!");
		return nullptr;
	}*/

	
	
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX: return CreateRef<DirectXVertexBuffer>(size, usage);
		}

		HL_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
	
	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX: return CreateRef<DirectXVertexBuffer>(data, size, usage);
		}

		HL_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}