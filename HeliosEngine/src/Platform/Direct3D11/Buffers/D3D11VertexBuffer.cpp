#include "D3D11VertexBuffer.h"
#include "Platform/Direct3D11/Direct3D11Context.h"

namespace Helios
{
	D3D11VertexBuffer::D3D11VertexBuffer(const void* data, uint32_t size, BufferUsage usage) : m_Usage(usage)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = size;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		switch(usage)
		{
		case BufferUsage::Stream:
		case BufferUsage::Dynamic:
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case BufferUsage::Static:
		default:
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.CPUAccessFlags = 0;
			break;
		}

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = data;

		HRESULT hr;
        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, &sd, &m_Buffer)),
            std::string("Failed to create Vertex Buffer!\n") + GetLastErrorAsString(hr) + "\n" + GetLastErrorAsString(Direct3D11Context::GetCurrentContext()->GetDevice()->GetDeviceRemovedReason())
        );
	}
	
	D3D11VertexBuffer::D3D11VertexBuffer(uint32_t size, BufferUsage usage) : m_Usage(usage)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.ByteWidth = size;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		switch(usage)
		{
		case BufferUsage::Stream:
		case BufferUsage::Dynamic:
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		case BufferUsage::Static:
		default:
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.CPUAccessFlags = 0;
			break;
		}

		HL_EXCEPTION(
            FAILED(Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, nullptr, &m_Buffer)),
            "Failed to create Vertex Buffer!"
        );
	}

	void D3D11VertexBuffer::Bind(uint32_t slot) const
	{
        uint32_t offset = 0;
		Direct3D11Context::GetCurrentContext()->GetContext()->IASetVertexBuffers(slot, 1, m_Buffer.GetAddressOf(), &m_Stride, &offset);
	}

	void D3D11VertexBuffer::Unbind(uint32_t slot) const
	{
		Direct3D11Context::GetCurrentContext()->GetContext()->IASetVertexBuffers(slot, 0, nullptr, nullptr, nullptr);
	}

	void D3D11VertexBuffer::SetData(const void* data, uint32_t size)
	{
        HL_ASSERT(m_Usage != BufferUsage::Static, "Buffer is not dynamic!");

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Direct3D11Context::GetCurrentContext()->GetContext()->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		std::memcpy(mappedData.pData, data, size);
		Direct3D11Context::GetCurrentContext()->GetContext()->Unmap(m_Buffer.Get(), 0);
	}

	void D3D11VertexBuffer::SetInputLayout(const InputLayout& layout)
	{
		m_InputLayout = layout;
		m_Stride = m_InputLayout.GetStride();
	}

	const InputLayout& D3D11VertexBuffer::GetInputLayout() const
	{
		return m_InputLayout;
	}

}