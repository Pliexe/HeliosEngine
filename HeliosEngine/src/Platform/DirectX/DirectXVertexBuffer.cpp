#include "Helios/Core/Asserts.h"
#include "DirectXVertexBuffer.h"
#include "Helios/Graphics/Graphics.h"
#include <d3d11.h>

namespace Helios
{
	DirectXVertexBuffer::DirectXVertexBuffer(uint32_t size, BufferUsage usage)
    {
        m_Usage = usage;
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.ByteWidth = size;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        switch (usage)
        {
        case BufferUsage::Static:
            bd.Usage = D3D11_USAGE_IMMUTABLE;
            bd.CPUAccessFlags = 0;
            break;
        case BufferUsage::Dynamic:
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            break;
        default:
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.CPUAccessFlags = 0;
            break;
        }

        HL_EXCEPTION(
            FAILED(Graphics::instance->m_device->CreateBuffer(&bd, nullptr, &m_VertexBuffer)),
            "Failed to create Vertex Buffer!"
        );
    }

    DirectXVertexBuffer::DirectXVertexBuffer(const void* data, uint32_t size, BufferUsage usage)
    {
        m_Usage = usage;
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.ByteWidth = size;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        switch (usage)
        {
        case BufferUsage::Static:
            bd.Usage = D3D11_USAGE_IMMUTABLE;
            bd.CPUAccessFlags = 0;
            break;
        case BufferUsage::Dynamic:
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            break;
        default:
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.CPUAccessFlags = 0;
            break;
        }

        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = data;

        HRESULT hr;
        HL_EXCEPTION(
            FAILED(hr = Graphics::instance->m_device->CreateBuffer(&bd, &sd, &m_VertexBuffer)),
            std::string("Failed to create Vertex Buffer!\n") + GetLastErrorAsString(hr) + "\n" + GetLastErrorAsString(Graphics::instance->m_device->GetDeviceRemovedReason())
        );
    }

    void DirectXVertexBuffer::Bind(uint32_t slot) const
    {
        uint32_t offset = 0;
        Graphics::instance->m_deviceContext->IASetVertexBuffers(slot, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
    }

    void DirectXVertexBuffer::Unbind() const
    {
        Graphics::instance->m_deviceContext->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
    }

    void DirectXVertexBuffer::SetData(const void* data, uint32_t size)
    {
        HL_CORE_ASSERT_WITH_MSG(m_Usage == BufferUsage::Dynamic, "Buffer is not dynamic!");

        D3D11_MAPPED_SUBRESOURCE ms;
        Graphics::instance->m_deviceContext->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, data, size);
        Graphics::instance->m_deviceContext->Unmap(m_VertexBuffer.Get(), 0);
    }
}