#pragma once

#include "pch.h"
#include "Helios/Resources/Buffer.h"

#ifdef HELIOS_PLATFORM_WINDOWS

namespace Helios
{
	class D3D11UniformBuffer : public UnsafeUniformBuffer
	{
	public:
		D3D11UniformBuffer(uint32_t binding_slot, uint32_t size);
		D3D11UniformBuffer(uint32_t binding_slot, const void* data, uint32_t size);
		~D3D11UniformBuffer() { }
		
		void SetData(const void* data) override;

		void Bind() const override;
		void Unbind() const override;
		
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
		uint32_t m_Size;
		uint32_t m_BindingSlot;
	};
}

#endif