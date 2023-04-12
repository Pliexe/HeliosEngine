#pragma once
#include "Helios/Translation/Vector.h"

namespace Helios
{
	class Mesh;
	struct HELIOS_API MeshVertex
	{
		Vector3 position;
		Vector2 texCoord;
		Vector3 normal;
	};
	class HELIOS_API MeshBuilder
	{
	public:

		struct Triangle
		{
			uint32_t i0, i1, i2;
		};
	private:
		std::vector<MeshVertex> m_Vertices;
		std::vector<Triangle> m_Triangles;
	public:
		
		MeshBuilder();
		MeshBuilder(uint32_t reservedVertexCount, uint32_t reservedTriangleCount);

		void Reserve(uint32_t vertexCount, uint32_t triangleCount);
		inline uint32_t AddVertex(MeshVertex vertex);
		inline uint32_t AddVertex(Vector3 position, Vector2 texCoord, Vector3 normal);
		void AddTriangle(uint32_t index0, uint32_t index1, uint32_t index2);
		void AddQuad(uint32_t index0, uint32_t index1, uint32_t index2, uint32_t index3);
		void CreateQuadFace(MeshVertex v0, MeshVertex v1, MeshVertex v2, MeshVertex v3);
		void CreateTriangleFace(MeshVertex v0, MeshVertex v1, MeshVertex v2);

		std::vector<MeshVertex> GetVertices() const;
		std::vector<Triangle> GetTriangles() const;

		friend class Mesh;
	};
}
