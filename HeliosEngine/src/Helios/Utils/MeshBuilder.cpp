﻿#include "MeshBuilder.h"

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

	uint32_t MeshBuilder::AddVertex(Vector3 position, Vector2 texCoord, Vector3 normal)
	{
		m_Vertices.emplace_back(MeshVertex{ position, texCoord, normal });
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

	void MeshBuilder::CreateQuadFace(MeshVertex v0, MeshVertex v1, MeshVertex v2, MeshVertex v3)
	{
		auto index0 = AddVertex(v0);
		auto index2 = AddVertex(v2);

		m_Triangles.emplace_back(Triangle{ index0, AddVertex(v1), index2 });
		m_Triangles.emplace_back(Triangle{ index0, index2, AddVertex(v3) });
	}

	void MeshBuilder::CreateTriangleFace(MeshVertex v0, MeshVertex v1, MeshVertex v2)
	{
		m_Triangles.emplace_back(Triangle{ AddVertex(v0), AddVertex(v1), AddVertex(v2) });
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
		return Vector3::Cross(v1.position - v0.position, v2.position - v0.position).normalize();
	}

	MeshVertex MeshBuilder::CalculateVertexNormal(std::vector<MeshVertex> m_Vertices, std::vector<Triangle> m_Triangles, const uint32_t index)
	{
		// Calulcate vertex normal from all connected triangles and return the average
		Vector3 normal = Vector3::Zero();

		for (const auto& triangle : m_Triangles)
		{
			if (triangle.i0 == index || triangle.i1 == index || triangle.i2 == index)
			{
				normal += CalculateNormal(m_Vertices[triangle.i0], m_Vertices[triangle.i1], m_Vertices[triangle.i2]);
			}
		}

		return MeshVertex{ Vector3::Zero(), Vector2::Zero(), normal };
	}

	void MeshBuilder::Reserve(uint32_t vertexCount, uint32_t triangleCount)
	{
		m_Vertices.reserve(vertexCount);
		m_Triangles.reserve(triangleCount);
	}
}
