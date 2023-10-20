#include "D3D11VertexArray.h"
#include "D3D11VertexBuffer.h"

namespace Helios
{
	//D3D11VertexArray::D3D11VertexArray(const InputLayouts& input_layouts)
	//{
	//	/*int i = 0;
	//	for (auto& input_layout : input_layouts)
	//	{
	//		Ref<D3D11VertexBuffer> vertex_buffer = CreateRef<D3D11VertexBuffer>(input_layout);
	//		i++;
	//	}*/
	//}

	D3D11VertexArray::D3D11VertexArray(const InputLayouts& input_layouts, std::vector<BufferSpecification> bufferSpecifications) : m_InputLayouts(input_layouts)
	{
		assert(bufferSpecifications.size() == input_layouts.size());

		// TODO: Use std::views::zip when c++23 is available

		// Iterate over input_layouts and bufferSpecifications and create vertex buffers based on the BufferSpecifications
		for (size_t i = 0; i < input_layouts.size(); i++)
		{
			Ref<D3D11VertexBuffer> vertex_buffer;

			if (bufferSpecifications[i].data == nullptr)
				vertex_buffer = CreateRef<D3D11VertexBuffer>(bufferSpecifications[i].size, bufferSpecifications[i].usage);
			else
				vertex_buffer = CreateRef<D3D11VertexBuffer>(bufferSpecifications[i].data, bufferSpecifications[i].size, bufferSpecifications[i].usage);

			vertex_buffer->m_BoundSlot = i;
			vertex_buffer->SetInputLayout(input_layouts[i]);

			m_VertexBuffers.push_back(vertex_buffer);
		}

	}

	void D3D11VertexArray::Bind() const
	{
		assert(m_IndexBuffer == nullptr);

		if (m_IndexBuffer != nullptr)
			m_IndexBuffer->Bind();

		for (auto& vertex_buffer : m_VertexBuffers)
			vertex_buffer->Bind();
	}

	void D3D11VertexArray::Unbind() const
	{
		assert(m_IndexBuffer == nullptr);

		if (m_IndexBuffer != nullptr)
			m_IndexBuffer->Unbind();

		for (auto& vertex_buffer : m_VertexBuffers)
			vertex_buffer->Unbind();
	}

	/*void D3D11VertexArray::AddVertexBuffer(const Ref<UnsafeVertexBuffer>& vertex_buffer)
	{
		std::static_pointer_cast<D3D11VertexBuffer>(vertex_buffer)->m_BoundSlot = m_VertexBuffers.size();
		m_VertexBuffers.push_back(vertex_buffer);
	}*/

	void D3D11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer)
	{
		m_IndexBuffer = index_buffer;
	}

	const std::vector<Ref<UnsafeVertexBuffer>>& D3D11VertexArray::GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	const Ref<IndexBuffer>& D3D11VertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}
	const InputLayouts& D3D11VertexArray::GetInputLayouts() const
	{
		return m_InputLayouts;
	}
}