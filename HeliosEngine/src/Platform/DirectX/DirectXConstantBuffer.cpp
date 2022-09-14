#include "Helios/Core/Asserts.h"
#include "DirectXConstantBuffer.h"

namespace Helios
{
    DirectXConstantBuffer::DirectXConstantBuffer(uint32_t size)
    {
        D3D11_BUFFER_DESC bd = { };
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0u;
        bd.ByteWidth = size;
        bd.StructureByteStride = 0u;

        HL_EXCEPTION(
            FAILED(Graphics::instance->m_device->CreateBuffer(&bd, nullptr, &m_Data)),
            "Failed to create Constant Buffer!"
        );
    }

	DirectXConstantBuffer::DirectXConstantBuffer(const void* data, uint32_t size)
    {
        D3D11_BUFFER_DESC bd = { };
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0u;
        bd.ByteWidth = size;
        bd.StructureByteStride = 0u;
        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = data;

        HL_EXCEPTION(
            FAILED(Graphics::instance->m_device->CreateBuffer(&bd, &sd, &m_Data)),
            "Failed to create Constant Buffer!"
        );
    }

    DirectXConstantBuffer::~DirectXConstantBuffer()
    {
        // WRL smart pointers will automatically release the COM object
    }

    void DirectXConstantBuffer::Bind(uint32_t slot) const
    {
        //Graphics::instance->m_deviceContext->VSSetConstantBuffers()
        Graphics::instance->m_deviceContext->VSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        // Graphics::instance->m_deviceContext->PSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
    }

    void DirectXConstantBuffer::Unbind() const
    {
        Graphics::instance->m_deviceContext->VSSetConstantBuffers(0u, 1u, nullptr);
        // Graphics::instance->m_deviceContext->PSSetConstantBuffers(0u, 1u, nullptr);
    }

    void DirectXConstantBuffer::SetData(const void* data, uint32_t size)
    {
        D3D11_MAPPED_SUBRESOURCE msr;
        Graphics::instance->m_deviceContext->Map(m_Data.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
        memcpy(msr.pData, data, size);
        Graphics::instance->m_deviceContext->Unmap(m_Data.Get(), 0u);
    }
}