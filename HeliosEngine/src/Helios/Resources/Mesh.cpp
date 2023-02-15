#include "Mesh.h"

namespace Helios
{
	//using MeshVertex;

	Ref<Mesh> Mesh::GetCubeMesh()
	{
		static const MeshVertex cubeVertices[] = {
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

		static uint32_t cubeIndices[] = {
			0 ,2 ,1, 2 ,3,1, // front face
			4 ,5 ,7, 4 ,7,6, // back face
			11,10,9, 10,8,9, // right face
			12 ,13 ,14, 14 ,13,15, // left face
			16 ,17 ,18, 18 ,17,19, // top face
			0 ,1 ,4, 1 ,5,4, // bottom face
		};

		return Mesh::Create("Cube", cubeVertices, std::size(cubeVertices), cubeIndices, std::size(cubeIndices));
	}

	Ref<Mesh> Mesh::GetCylinderMesh()
	{
		static Ref<Mesh> cylinderMesh = GetDynamicCylinderMesh(32);
		return cylinderMesh;
	}

	Ref<Mesh> Mesh::GetConeMesh()
	{
		static Ref<Mesh> coneMesh = GetDynamicConeMesh(32);
		return coneMesh;
	}

	Ref<Mesh> Mesh::GetSphereMesh()
	{
		static Ref<Mesh> sphereMesh = GetDynamicSphereMesh(32);
		return sphereMesh;
	}

	Ref<Mesh> Mesh::GetDynamicSphereMesh(uint32_t segments)
	{
		MeshBuilder builder(segments * segments, segments * segments * 6);

		auto vtop = builder.AddVertex({ 0.0f, 1.0f, 0.0f }, { 0.5f, 0.0f });
		auto vbottom = builder.AddVertex({ 0.0f, -1.0f, 0.0f }, { 0.5f, 1.0f });

		// Go trough segments in horizontal direction.
		for (uint32_t i = 0; i < segments; i++)
		{
			// Go trough segments in vertical direction and skip the top and bottom vertices.
			for (uint32_t j = 1; j < segments - 1; j++)
			{
				float x = cosf((float)i / (float)segments * 2.0f * (float)M_PI) * sinf((float)j / (float)segments * (float)M_PI);
				float y = cosf((float)j / (float)segments * (float)M_PI);
				float z = sinf((float)i / (float)segments * 2.0f * (float)M_PI) * sinf((float)j / (float)segments * (float)M_PI);

				auto v = builder.AddVertex({ x, y, z }, { (float)i / (float)segments, (float)j / (float)segments });
			}
		}

		// Create small white dots where the current vertices are.

		const float inner_segments = segments - 2;

		// Each segment in the horizontal direction 
		for (uint32_t i = 0; i < segments - 1; i++)
		{
			// Connect to top
			builder.AddTriangle(vtop, (i + 1) * inner_segments + 2, i * inner_segments + 2);

			// Connect segments in vertical direction as quads and skip the top and bottom vertices.
			for (uint32_t j = 1; j < segments - 2; j++)
			{
				builder.AddQuad((i + 1) * inner_segments + j + 2, i * inner_segments + j + 2, i * inner_segments + j + 1, (i + 1) * inner_segments + j + 1);
			}

			// Connect to bottom
			builder.AddTriangle(vbottom, i * inner_segments + 5, (i + 1) * inner_segments + 5);
		}

		builder.AddTriangle(vtop, 2, (segments - 1) * inner_segments + 2);
		for (uint32_t j = 1; j < segments - 2; j++)
		{
			builder.AddQuad(j + 2, (segments - 1) * inner_segments + j + 2, (segments - 1) * inner_segments + j + 1, j + 1);
		}
		builder.AddTriangle(vbottom, (segments - 1) * inner_segments + 5, 5);
		
		return Mesh::Create("Sphere_" + std::to_string(segments), builder);
	}

	Ref<Mesh> Mesh::GetDynamicConeMesh(uint32_t segments)
	{
		MeshVertex* vertices = new MeshVertex[segments + 2];
		uint32_t* indices = new uint32_t[segments * 3 * 2];
		ZeroMemory(indices, sizeof(indices));

		vertices[0] = { { 0.0f,-1.0f, 0.0f }, { 0.5f, 0.5f } };
		vertices[1] = { { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } };

		for (uint32_t i = 0; i < segments; i++)
		{
			// Vertices
			float angle = (float)i / (float)segments * 2.0f * 3.14159265359f;
			float x = cos(angle);
			float z = sin(angle);

			vertices[i + 2] = { { x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f } };
			
			// Indices
			indices[i * 3] = 0;
			indices[i * 3 + 1] = i + 2;
			indices[i * 3 + 2] = (segments - 1) == i ? 2 : i + 3;

			indices[(i * 3 + segments * 3) + 2] = i + 2;
			indices[(i * 3 + segments * 3) + 1] = (segments - 1) == i ? 2 : i + 3;
			indices[(i * 3 + segments * 3)] = 1;
		}

		return Mesh::Create("Cone_" + std::to_string(segments), vertices, segments + 2, indices, segments * 3 * 2);
	}

	Ref<Mesh> Mesh::GetDynamicCylinderMesh(uint32_t segments)
	{
		MeshVertex *vertices = new MeshVertex[segments * 2 + 2];
		uint32_t* indices = new uint32_t[segments * 3 * 4];
		ZeroMemory(indices, sizeof(indices));

		vertices[0] = { { 0.0f,-1.0f, 0.0f }, { 0.5f, 0.5f } };
		vertices[1] = { { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } };
		
		for (uint32_t i = 0; i < segments; i++)
		{
			// Vertices
			float angle = (float)i / (float)segments * 2.0f * 3.14159265359f;
			float x = cos(angle);
			float z = sin(angle);

			vertices[i + 2] = { { x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f } };
			vertices[i + 2 + segments] = { { x, 1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f } };

			// Indices
			indices[i * 3]     = 0;
			indices[i * 3 + 1] = i + 2;
			indices[i * 3 + 2] = (segments - 1) == i ? 2 : i + 3;

			indices[(i * 6 + segments * 3) + 2] = i + 2;
			indices[(i * 6 + segments * 3) + 1] = (segments - 1) == i ? 2 : i + 3;
			indices[(i * 6 + segments * 3)] = i + 2 + segments;

			indices[(i * 6 + segments * 3) + 4] = (segments - 1) == i ? 2 + segments : i + 3 + segments;
			indices[(i * 6 + segments * 3) + 5] = (segments - 1) == i ? 2 : i + 3;
			indices[(i * 6 + segments * 3) + 3] = i + 2 + segments;
							   
			indices[(i * 3 + segments * 3 * 3)] = 1;
			indices[(i * 3 + segments * 3 * 3) + 2] = segments + i + 2;
			indices[(i * 3 + segments * 3 * 3) + 1] = (segments - 1) == i ? segments + 2 : segments + i + 3;
		}
		
		return Mesh::Create("Cylinder_" + std::to_string(segments), vertices, segments * 2 + 2, indices, segments * 3 * 4);
	}
	
	std::unordered_map<std::string, Ref<Mesh>> Mesh::s_Meshes;
	
	Mesh::Mesh(std::string name, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount)
	{
		m_Vertices = std::vector<MeshVertex>(vertices, vertices + vertexCount);
		m_VertexBuffer = VertexBuffer::Create(vertices, vertexCount * sizeof(MeshVertex));
		m_VertexBuffer->SetStride<MeshVertex>();
		m_IndexBuffer = IndexBuffer::Create(indices, indexCount);
	}

	Mesh::Mesh(std::string name, const MeshBuilder& meshBuilder)
	{
		m_Vertices = std::vector<MeshVertex>(meshBuilder.m_Vertices);
		m_VertexBuffer = VertexBuffer::Create(meshBuilder.m_Vertices.data(), meshBuilder.m_Vertices.size() * sizeof(MeshVertex));
		m_VertexBuffer->SetStride<MeshVertex>();
		m_IndexBuffer = IndexBuffer::Create((uint32_t*)meshBuilder.m_Triangles.data(), meshBuilder.m_Triangles.size() * 3);
	}

	Ref<Mesh> Mesh::Create(std::string name, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount)
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

	Ref<Mesh> Mesh::Create(std::string name, const MeshBuilder& meshBuilder)
	{
		if (s_Meshes.find(name) != s_Meshes.end())
		{
			return s_Meshes[name];
		}
		else
		{
			Ref<Mesh> mesh = CreateRef<Mesh>(name, meshBuilder);
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