#include "D3D11UniformBuffer.h"
#include "Helios/Core/Asserts.h"
#include "Platform/Direct3D11/Direct3D11Context.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
	D3D11UniformBuffer::D3D11UniformBuffer(uint32_t binding_slot, uint32_t size) : m_Size(size), m_BindingSlot(binding_slot)
	{
		assert(size % 16 == 0);
		assert(binding_slot >= 0);

		D3D11_BUFFER_DESC bufferDesc;
		SecureZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.ByteWidth = size;
		bufferDesc.StructureByteStride = 0u;

		HRESULT hr;
		HL_EXCEPTION(
			FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_Buffer)),
			std::string("Failed to create Uniform Buffer (Constant Buffer)!") + "\nError: " + GetLastErrorAsString(hr)
		);
	}

	D3D11UniformBuffer::D3D11UniformBuffer(uint32_t binding_slot, const void* data, uint32_t size) : m_Size(size), m_BindingSlot(binding_slot)
	{
		assert(data == nullptr);
		assert(size % 16 == 0);
		assert(binding_slot >= 0);

		D3D11_BUFFER_DESC bufferDesc;
		SecureZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.ByteWidth = size;
		bufferDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA subresourceData;
		ZeroMemory(&subresourceData, sizeof(subresourceData));
		subresourceData.pSysMem = data;

		HRESULT hr;
		HL_EXCEPTION(
			FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &m_Buffer)),
			std::string("Failed to create Constant Buffer!") + "\nError: " + GetLastErrorAsString(hr)
		);
	}

	void D3D11UniformBuffer::SetData(const void* data)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		HRESULT hr;
		HL_EXCEPTION_HR(
			FAILED(hr = Direct3D11Context::GetCurrentContext()->GetContext()->Map(m_Buffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource)),
			std::string("Failed to map Constant Buffer!") + "\nError: " + GetLastErrorAsString(hr), hr
		);

		std::memcpy(mappedResource.pData, data, m_Size);

		Direct3D11Context::GetCurrentContext()->GetContext()->Unmap(m_Buffer.Get(), 0u);
	}
	void D3D11UniformBuffer::Bind() const
	{
		Direct3D11Context::GetCurrentContext()->GetContext()->VSSetConstantBuffers(m_BindingSlot, 1u, m_Buffer.GetAddressOf());
		Direct3D11Context::GetCurrentContext()->GetContext()->PSSetConstantBuffers(m_BindingSlot, 1u, m_Buffer.GetAddressOf());
		//Direct3D11Context::GetCurrentContext()->GetContext()->CSSetConstantBuffers(m_BindingSlot, 1u, m_Buffer.GetAddressOf());
		Direct3D11Context::GetCurrentContext()->GetContext()->GSSetConstantBuffers(m_BindingSlot, 1u, m_Buffer.GetAddressOf());
		//Direct3D11Context::GetCurrentContext()->GetContext()->HSSetConstantBuffers(m_BindingSlot, 1u, m_Buffer.GetAddressOf());
		//Direct3D11Context::GetCurrentContext()->GetContext()->DSSetConstantBuffers(m_BindingSlot, 1u, m_Buffer.GetAddressOf());
	}
	void D3D11UniformBuffer::Unbind() const
	{
		Direct3D11Context::GetCurrentContext()->GetContext()->VSSetConstantBuffers(m_BindingSlot, 0u, nullptr);
		Direct3D11Context::GetCurrentContext()->GetContext()->PSSetConstantBuffers(m_BindingSlot, 0u, nullptr);
		//Direct3D11Context::GetCurrentContext()->GetContext()->CSSetConstantBuffers(m_BindingSlot, 0u, nullptr);
		Direct3D11Context::GetCurrentContext()->GetContext()->GSSetConstantBuffers(m_BindingSlot, 0u, nullptr);
		//Direct3D11Context::GetCurrentContext()->GetContext()->HSSetConstantBuffers(m_BindingSlot, 0u, nullptr);
		//Direct3D11Context::GetCurrentContext()->GetContext()->DSSetConstantBuffers(m_BindingSlot, 0u, nullptr);
	}
}
#endif