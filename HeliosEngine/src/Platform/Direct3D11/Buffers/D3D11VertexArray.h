#pragma once

#include "pch.h"
#include "Helios/Resources/Buffer.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
	class D3D11VertexArray : public VertexArray
	{
	public:
		D3D11VertexArray(const InputLayouts& input_layouts, std::vector<BufferSpecification> bufferSpecifications);
		D3D11VertexArray(const InputLayouts& input_layouts, std::vector<BufferSpecification> bufferSpecifications, Ref<UnsafeVertexBuffer> instanceBuffer);
		D3D11VertexArray(const InputLayouts& inputLayouts, std::vector<Ref<UnsafeVertexBuffer>> buffers);

		void Bind() const override;

		void Unbind() const override;


		void SetIndexBuffer(const Ref<IndexBuffer>& index_buffer) override;

		const std::vector<Ref<UnsafeVertexBuffer>>& GetVertexBuffers() const override;

		const Ref<IndexBuffer>& GetIndexBuffer() const override;

	private:
		Ref<IndexBuffer> m_IndexBuffer;
		std::vector<Ref<UnsafeVertexBuffer>> m_VertexBuffers;
		InputLayouts m_InputLayouts;

		// Inherited via VertexArray
		const InputLayouts& GetInputLayouts() const override;
	};
}
#endif