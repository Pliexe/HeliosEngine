#include "MeshBuilder.h"

namespace Helios
{
	MeshBuilder::MeshBuilder() { }

	MeshBuilder::MeshBuilder(uint32_t reservedVertexCount, uint32_t reservedTriangleCount)
	{
		m_Vertices.reserve(reservedVertexCount);
		m_Triangles.reserve(reservedTriangleCount);
	}

	inline uint32_t MeshBuilder::AddVertex(MeshVertex vertex)
	{
		m_Vertices.emplace_back(vertex);
		return m_Vertices.size() - 1;
	}

	uint32_t MeshBuilder::AddVertex(Vector3 position, Vector2 texCoord)
	{
		m_Vertices.emplace_back(MeshVertex{ position, texCoord });
		return m_Vertices.size() - 1;
	}

	void MeshBuilder::AddTriangle(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		m_Triangles.emplace_back(Triangle{ index0, index1, index2 });
	}

	void MeshBuilder::AddQuad(uint32_t index0, uint32_t index1, uint32_t index2, uint32_t index3)
	{
		m_Triangles.emplace_back(Triangle{ index0, index1, index2 });
		m_Triangles.emplace_back(Triangle{ index0, index2, index3 });
	}

	void MeshBuilder::Reserve(uint32_t vertexCount, uint32_t triangleCount)
	{
		m_Vertices.reserve(vertexCount);
		m_Triangles.reserve(triangleCount);
	}
}
