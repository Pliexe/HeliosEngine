#pragma once

#include "Platform/DirectX/DirectXConstantBuffer.h"
#include "Buffer.h"
#include "Platform/DirectX/Test.h"

namespace Helios
{

	template <class T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create()
	{
		switch (Graphics::GetAPI())
		{
			case Graphics::API::DirectX11: return (CreateRef<DirectXConstantBuffer<T>>(sizeof(T) + (16 - sizeof(T) % 16)));
		}

		HL_CORE_ASSERT(false, "Unknown Graphics API!");
		return nullptr;
	}

	template <class T>
	Ref<ConstantBuffer<T>> ConstantBuffer<T>::Create(T data)
	{

		switch (Graphics::GetAPI())
		{
			case Graphics::API::DirectX11: return CreateRef<DirectXConstantBuffer<T>>(data, sizeof(T) + (16 - sizeof(T) % 16));
		}

		HL_CORE_ASSERT(false, "Unknown GraphicsAPI!");
		return nullptr;
	}
}