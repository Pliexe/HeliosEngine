#include "Buffer.h"
#include "Platform/DirectX/DirectXConstantBuffer.h"
#include "Platform/DirectX/DirectXVertexBuffer.h"

namespace Helios
{
	Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size)
	{
		HL_ASSERT_EXCEPTION(!(size % 16), "Constant buffer size must be a multiple of 16 bytes!\nClass Name: " + std::string(typeid(ConstantBuffer).name()));
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX: return CreateRef<DirectXConstantBuffer>(size);
		}

		HL_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<ConstantBuffer> ConstantBuffer::Create(const void* data, uint32_t size)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::DirectX: return CreateRef<DirectXConstantBuffer>(data, size);
		}

		HL_CORE_ASSERT(false, "Unknown GraphicsAPI!");
		return nullptr;
	}
	
	
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