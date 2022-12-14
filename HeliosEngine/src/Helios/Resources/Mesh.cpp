#include "Mesh.h"

namespace Helios
{
	Ref<Mesh> Mesh::GetCubeMesh()
	{
		static const MeshVertexData cubeVertices[] = {
			// Front Face
			{ {-0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f } },
			{ { 0.5f, -0.5f, -0.5f}, { 1.0f, 1.0f } },
			{ {-0.5f,  0.5f, -0.5f}, { 0.0f, 0.0f } },
			{ { 0.5f,  0.5f, -0.5f}, { 1.0f, 0.0f } },

			// Back Face
			{ {-0.5f, -0.5f,  0.5f}, { 1.0f, 1.0f } },
			{ { 0.5f, -0.5f,  0.5f}, { 0.0f, 1.0f } },
			{ {-0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f } },
			{ { 0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f } },

			// Right Face
			{ { 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f } },
			{ { 0.5f, -0.5f,  0.5f}, { 1.0f, 1.0f } },
			{ { 0.5f,  0.5f, -0.5f}, { 0.0f, 0.0f } },
			{ { 0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f } },

			// Left Face
			{ {-0.5f, -0.5f, -0.5f}, { 1.0f, 1.0f } },
			{ {-0.5f, -0.5f,  0.5f}, { 0.0f, 1.0f } },
			{ {-0.5f,  0.5f, -0.5f}, { 1.0f, 0.0f } },
			{ {-0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f } },

			// Top Face
			{ {-0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f } },
			{ {-0.5f,  0.5f,  0.5f}, { 0.0f, 1.0f } },
			{ { 0.5f,  0.5f, -0.5f}, { 0.0f, 0.0f } },
			{ { 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f } },

			// Bottom face
		};

		// TODO: create more vertecies for UV data so the cube is rendered properly.

		static const unsigned short cubeIndices[] = {
			0 ,2 ,1, 2 ,3,1, // front face
			4 ,5 ,7, 4 ,7,6, // back face
			11,10,9, 10,8,9, // right face
			12 ,13 ,14, 14 ,13,15, // left face
			16 ,17 ,18, 18 ,17,19, // top face
			0 ,1 ,4, 1 ,5,4, // bottom face
		};

		return Mesh::Create("Cube", cubeVertices, std::size(cubeVertices), cubeIndices, std::size(cubeIndices));
	}
	
	std::unordered_map<std::string, Ref<Mesh>> Mesh::s_Meshes;
	
	Mesh::Mesh(std::string name, const MeshVertexData* vertices, uint32_t vertexCount, const unsigned short* indices, uint32_t indexCount)
	{
		m_VertexBuffer = VertexBuffer::Create(vertices, vertexCount * sizeof(MeshVertexData));
		m_VertexBuffer->SetStride<MeshVertexData>();
		m_IndexBuffer = IndexBuffer::Create(indices, indexCount);
	}

	Ref<Mesh> Mesh::Create(std::string name, const MeshVertexData* vertices, uint32_t vertexCount, const unsigned short* indices, uint32_t indexCount)
	{
		if (s_Meshes.find(name) != s_Meshes.end())
		{
			return s_Meshes[name];
		}
		else
		{
			Ref<Mesh> mesh = CreateRef<Mesh>(name, vertices, vertexCount, indices, indexCount);
			s_Meshes[name] = mesh;
			return mesh;
		}
	}

	void Mesh::Destroy(std::string name)
	{
		if (s_Meshes.find(name) != s_Meshes.end())
		{
			s_Meshes.erase(name);
		}
	}

	void Mesh::Bind()
	{
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
	}
}