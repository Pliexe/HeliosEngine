#include "Helios/Core/Asserts.h"
#include "DirectXVertexBuffer.h"
#include "Helios/Graphics/DepricatedGraphics.h"
#include <d3d11.h>

#include "Direct3D11Context.h"

namespace Helios
{
	DepricatedDirectXVertexBuffer::DepricatedDirectXVertexBuffer(uint32_t size, BufferUsage usage)
    {
        m_Usage = usage;
		m_Size = size;
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
            FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, nullptr, &m_VertexBuffer)),
            "Failed to create Vertex Buffer!"
        );
    }

    DepricatedDirectXVertexBuffer::DepricatedDirectXVertexBuffer(const void* data, uint32_t size, BufferUsage usage)
    {
        m_Usage = usage;
		m_Size = size;
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
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer)),
            std::string("Failed to create Vertex Buffer!\n") + GetLastErrorAsString(hr) + "\n" + GetLastErrorAsString(Direct3D11Context::GetCurrentContext()->GetDevice()->GetDeviceRemovedReason())
        );
    }

    void DepricatedDirectXVertexBuffer::Bind(uint32_t slot) const
    {
        uint32_t offset = 0;
        Direct3D11Context::GetCurrentContext()->GetContext()->IASetVertexBuffers(slot, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
    }

    void DepricatedDirectXVertexBuffer::Unbind() const
    {
        Direct3D11Context::GetCurrentContext()->GetContext()->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
    }

    void DepricatedDirectXVertexBuffer::SetData(const void* data, uint32_t size)
    {
        HL_CORE_ASSERT_WITH_MSG(m_Usage == BufferUsage::Dynamic, "Buffer is not dynamic!");

        D3D11_MAPPED_SUBRESOURCE ms;
        Direct3D11Context::GetCurrentContext()->GetContext()->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, data, size);
        Direct3D11Context::GetCurrentContext()->GetContext()->Unmap(m_VertexBuffer.Get(), 0);
        m_Size = size;
    }
}
