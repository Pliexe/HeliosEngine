#include "Buffer.h"

#include "Helios/Graphics/Graphics.h"
#include "Platform/Direct3D11/DirectXConstantBuffer.h"
#include "Platform/Direct3D11/DirectXVertexBuffer.h"

namespace Helios
{
	/*template <typename T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create()
	{
		switch (DepricatedGraphics::GetAPI())
		{
		case DepricatedGraphics::API::DirectX11: return CreateRef<DirectXConstantBuffer>(sizeof(T) + (16 - sizeof(T) % 16));
		}

		HELIOS_ASSERT(false, "Unknown DepricatedGraphics API!");
		return nullptr;
	}

	template <typename T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create(T data)
	{
		switch (DepricatedGraphics::GetAPI())
		{
		case DepricatedGraphics::API::DirectX11: return CreateRef<DirectXConstantBuffer>(data, sizeof(T) + (16 - sizeof(T) % 16));
		}

		HELIOS_ASSERT(false, "Unknown GraphicsAPI!");
		return nullptr;
	}*/

	
	
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<DirectXVertexBuffer>(size, usage);
		}

		HELIOS_ASSERT(false, "Unknown DepricatedGraphics API!");
		return nullptr;
	}
	
	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
	{
		switch (Graphics::GetAPI())
		{
		case Graphics::API::Direct3D11: return CreateRef<DirectXVertexBuffer>(data, size, usage);
		}

		HELIOS_ASSERT(false, "Unknown DepricatedGraphics API!");
		return nullptr;
	}
}
