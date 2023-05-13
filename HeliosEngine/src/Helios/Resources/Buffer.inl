#pragma once

#include "Platform/Direct3D11/DirectXConstantBuffer.h"
#include "Buffer.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{

	template <class T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create()
	{
		switch (Graphics::GetAPI())
		{
			case Graphics::API::Direct3D11: return (CreateRef<DirectXConstantBuffer<T>>(sizeof(T) + (16 - sizeof(T) % 16)));
		}

		HELIOS_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	template <class T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create(T data)
	{

		switch (Graphics::GetAPI())
		{
			case Graphics::API::Direct3D11: return CreateRef<DirectXConstantBuffer<T>>(data, sizeof(T) + (16 - sizeof(T) % 16));
		}

		HELIOS_ASSERT(false, "Unknown GraphicsAPI!");
		return nullptr;
	}
}
