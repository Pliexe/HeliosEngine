#include "MeshBuilder.h"

namespace Helios
{
	MeshBuilder::MeshBuilder() { }

	MeshBuilder::MeshBuilder(uint32_t reservedVertexCount, uint32_t reservedTriangleCount)
	{
		m_Vertices.reserve(reservedVertexCount);
		m_Triangles.reserve(reservedTriangleCount);
	}

	//inline uint32_t MeshBuilder::AddVertex(MeshVertex vertex)
	//{
	//	m_Vertices.emplace_back(std::forward<MeshVertex>(vertex));
	//	return m_Vertices.size() - 1;
	//}

	uint32_t MeshBuilder::AddVertex(MeshVertex&& vertex)
	{
		m_Vertices.emplace_back(std::forward<MeshVertex>(vertex));
		return m_Vertices.size() - 1;
	}

	uint32_t MeshBuilder::AddVertex(Vector3 position, Vector2 texCoord, Vector3 normal)
	{
		m_Vertices.emplace_back(MeshVertex{ std::forward<Vector3>(position), std::forward<Vector2>(texCoord), std::forward<Vector3>(normal) }); //MeshVertex{ position, texCoord, normal });
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

	void MeshBuilder::CreateQuadFace(MeshVertex& v0, MeshVertex& v1, MeshVertex& v2, MeshVertex& v3)
	{
		auto index0 = AddVertex(std::forward<MeshVertex>(v0));
		auto index2 = AddVertex(std::forward<MeshVertex>(v2));

		m_Triangles.emplace_back(Triangle{ std::forward<uint32_t>(index0), std::forward<uint32_t>(AddVertex(std::forward<MeshVertex>(v1))), std::forward<uint32_t>(index2) });
		m_Triangles.emplace_back(Triangle{ std::forward<uint32_t>(index0), std::forward<uint32_t>(index2), std::forward<uint32_t>(AddVertex(std::forward<MeshVertex>(v3))) });
	}

	void MeshBuilder::CreateQuadFace(MeshVertex&& v0, MeshVertex&& v1, MeshVertex&& v2, MeshVertex&& v3)
	{
		auto index0 = AddVertex(std::forward<MeshVertex>(v0));
		auto index2 = AddVertex(std::forward<MeshVertex>(v2));

		m_Triangles.emplace_back(Triangle{ std::forward<uint32_t>(index0), std::forward<uint32_t>(AddVertex(std::forward<MeshVertex>(v1))), std::forward<uint32_t>(index2) });
		m_Triangles.emplace_back(Triangle{ std::forward<uint32_t>(index0), std::forward<uint32_t>(index2), std::forward<uint32_t>(AddVertex(std::forward<MeshVertex>(v3))) });
	}

	void MeshBuilder::CreateTriangleFace(MeshVertex& v0, MeshVertex& v1, MeshVertex& v2)
	{
		m_Triangles.emplace_back(Triangle{ AddVertex(std::forward<MeshVertex>(v0)), AddVertex(std::forward<MeshVertex>(v1)), AddVertex(std::forward<MeshVertex>(v2)) }); //m_Triangles.emplace_back(Triangle{ AddVertex(v0), AddVertex(v1), AddVertex(v2) });
	}

	void MeshBuilder::CreateTriangleFace(MeshVertex&& v0, MeshVertex&& v1, MeshVertex&& v2)
	{
		m_Triangles.emplace_back(Triangle{ AddVertex(std::forward<MeshVertex>(v0)), AddVertex(std::forward<MeshVertex>(v1)), AddVertex(std::forward<MeshVertex>(v2)) }); //m_Triangles.emplace_back(Triangle{ AddVertex(v0), AddVertex(v1), AddVertex(v2) });
	}

	std::vector<MeshVertex>& MeshBuilder::GetVertices()
	{
		return m_Vertices;
	}

	std::vector<MeshBuilder::Triangle>& MeshBuilder::GetTriangles()
	{
		return m_Triangles;
	}

	Vector3 MeshBuilder::CalculateNormal(const MeshVertex& v0, const MeshVertex& v1, const MeshVertex& v2)
	{
		return Vector3::Cross(v1.position - v0.position, v2.position - v0.position).normalized();
	}

	void MeshBuilder::CalculateVertexNormals(MeshBuilder& meshBuilder)
	{
		for (size_t i = 0; i < meshBuilder.m_Triangles.size(); i++)
		{
			Vector3 normal = CalculateNormal(meshBuilder.m_Vertices[meshBuilder.m_Triangles[i].i0], meshBuilder.m_Vertices[meshBuilder.m_Triangles[i].i1], meshBuilder.m_Vertices[meshBuilder.m_Triangles[i].i2]);

			meshBuilder.m_Vertices[meshBuilder.m_Triangles[i].i0].normal += normal;
			meshBuilder.m_Vertices[meshBuilder.m_Triangles[i].i1].normal += normal;
			meshBuilder.m_Vertices[meshBuilder.m_Triangles[i].i2].normal += normal;
		}
	}

	void MeshBuilder::Reserve(uint32_t vertexCount, uint32_t triangleCount)
	{
		m_Vertices.reserve(vertexCount);
		m_Triangles.reserve(triangleCount);
	}

}
