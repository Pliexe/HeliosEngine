#pragma once
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/Color.h"

namespace Helios
{
	class StackedGraph
	{
		inline static uint32_t MAX_GRAPH_INDEX_BUFFER = 1000u;

	public:
		struct VertexGraph
		{
			Vector2 position;
			Color color;
		};

		//graphVertices->Bind();
		//graphIndices->Bind();
		//graphics->m_deviceContext->DrawIndexed(graphIndices->count(), 0u, 0u);

		StackedGraph()
		{
			vertexBuf = VertexBuffer::Create(sizeof(VertexGraph), BufferUsage::Dynamic);
			indexBuf = IndexBuffer::Create(MAX_GRAPH_INDEX_BUFFER);
		}
	private:
		Ref<VertexBuffer> vertexBuf;
		Ref<IndexBuffer> indexBuf;
	};
}

