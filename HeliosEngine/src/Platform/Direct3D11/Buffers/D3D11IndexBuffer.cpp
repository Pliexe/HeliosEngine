#include "D3D11IndexBuffer.h"
#include "D3D11Usage.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
    D3D11IndexBuffer::D3D11IndexBuffer(uint32_t* indices, uint32_t count, BufferUsage usage)
    {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

        bufferDesc.ByteWidth = sizeof(uint32_t) * count;
        bufferDesc.Usage = GetBufferUsage(usage);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = sizeof(uint32_t);

        D3D11_SUBRESOURCE_DATA subResourceData;
        ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        subResourceData.pSysMem = indices;

        HRESULT hr;
        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bufferDesc, &subResourceData, &m_buffer)),
            "D3D11IndexBuffer::D3D11IndexBuffer() failed"
        );

        m_count = count;
        m_data = new uint32_t[count];
        std::memcpy(m_data, indices, count * sizeof(uint32_t));
    }
    D3D11IndexBuffer::D3D11IndexBuffer(uint32_t count, BufferUsage usage)
    {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

        bufferDesc.ByteWidth = sizeof(uint32_t) * count;
        bufferDesc.Usage = GetBufferUsage(usage);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = sizeof(uint32_t);

        HRESULT hr;
        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer)),
            "D3D11IndexBuffer::D3D11IndexBuffer() failed"
        );

        m_count = count;
    }
    D3D11IndexBuffer::~D3D11IndexBuffer()
    {
        delete[] m_data;
    }

    void D3D11IndexBuffer::Bind() const
    {
        Direct3D11Context::GetCurrentContext()->GetContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    }
    void D3D11IndexBuffer::Unbind() const
    {
        Direct3D11Context::GetCurrentContext()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    }
    void D3D11IndexBuffer::SetData(const uint32_t* data, uint32_t count)
    {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

        bufferDesc.ByteWidth = sizeof(uint32_t) * count;
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subResourceData;
        ZeroMemory(&subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
        subResourceData.pSysMem = data;

        Direct3D11Context::GetCurrentContext()->GetContext()->UpdateSubresource(m_buffer.Get(), 0, nullptr, data, 0, 0);
    }
    uint32_t D3D11IndexBuffer::GetCount() const
    {
        return m_count;
    }
    std::string D3D11IndexBuffer::ToString() const
    {
        std::string str = std::to_string(m_data[0]);

        for (uint32_t i = 1; i < m_count; i++)
        {
         	str += (i % 3 == 0 ? "\n" : ", ") + std::to_string(m_data[i]);
        }

        return str;
    }
}
#endif