#include "Buffer.h"

#include "Helios/Graphics/Graphics.h"

#ifdef HELIOS_PLATFORM_WINDOWS

#include "Platform/Direct3D11/Buffers/D3D11UniformBuffer.h"
#include "Platform/Direct3D11/Buffers/D3D11IndexBuffer.h"
#include "Platform/Direct3D11/Buffers/D3D11VertexBuffer.h"

#endif
#include <Platform/Direct3D11/Buffers/D3D11VertexArray.h>


namespace Helios
{

	Ref<UnsafeUniformBuffer> UnsafeUniformBuffer::Create(uint32_t binding_slot, uint32_t size)
	{
		switch (Graphics::GetAPI())
		{
#ifdef HELIOS_PLATFORM_WINDOWS
		case Graphics::API::Direct3D11:
			return CreateRef<D3D11UniformBuffer>(binding_slot, size + (16 - size % 16));
			return nullptr;
#endif
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<UnsafeUniformBuffer> UnsafeUniformBuffer::Create(uint32_t binding_slot, const void* data, uint32_t size)
	{
		switch (Graphics::GetAPI())
		{
#ifdef HELIOS_PLATFORM_WINDOWS
			case Graphics::API::Direct3D11:
				return CreateRef<D3D11UniformBuffer>(binding_slot, data, size + (16 - size % 16));
#endif
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(std::initializer_list<uint32_t> indices, BufferUsage usage)
	{
		return IndexBuffer::Create((uint32_t*)indices.begin(), indices.size(), usage);
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11IndexBuffer>(indices, count, usage);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
	
	


	Ref<UnsafeVertexBuffer> UnsafeVertexBuffer::Create(uint32_t size, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11VertexBuffer>(size, usage);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<UnsafeVertexBuffer> UnsafeVertexBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11VertexBuffer>(data, size, usage);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
	Ref<VertexArray> VertexArray::Create(const InputLayouts& inputLayouts, std::initializer_list<BufferSpecification> bufferSpecifications)
	{
		HL_ASSERT(bufferSpecifications.size() == inputLayouts.size(), "BufferSpecifications and InputLayouts must be the same size!");

		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11VertexArray>(inputLayouts, bufferSpecifications);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	Ref<VertexArray> VertexArray::Create(const InputLayouts& inputLayouts, std::initializer_list<Ref<UnsafeVertexBuffer>> buffers)
	{
		HL_ASSERT(buffers.size() == inputLayouts.size(), "BufferSpecifications and InputLayouts must be the same size!");

		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11VertexArray>(inputLayouts, buffers);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
	Ref<VertexArray> VertexArray::Create(const InputLayouts& inputLayouts, std::initializer_list<BufferSpecification> bufferSpecifications, Ref<UnsafeVertexBuffer> instanceBuffer)
	{
		HL_ASSERT(bufferSpecifications.size() == inputLayouts.size(), "BufferSpecifications and InputLayouts must be the same size!");

		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<D3D11VertexArray>(inputLayouts, bufferSpecifications, instanceBuffer);
		}

		HL_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}
}
