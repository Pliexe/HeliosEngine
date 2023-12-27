#include "Mesh.h"

namespace Helios
{
	//using MeshVertex;

	Ref<Mesh> Mesh::GeneratePlane()
	{
		static const MeshVertex vertices[4] = {
			{ { -2.0f, 0.0f, -2.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { -2.0f, 0.0f, 2.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { 2.0f, 0.0f, 2.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { 2.0f, 0.0f, -2.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } }
		};

		static uint32_t triangles[6] = {
			0, 1, 2,
			0, 2, 3
		};

		return CreateRef<Mesh>("Plane", vertices, std::size(vertices), triangles, std::size(triangles));
	}

	Ref<Mesh> Mesh::GenerateCube()
	{
		MeshBuilder builder;

		// Front Face
		builder.CreateQuadFace(
			{ { -0.5f,  0.5f, -0.5 }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ {  0.5f,  0.5f, -0.5 }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ {  0.5f, -0.5f, -0.5 }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
			{ { -0.5f, -0.5f, -0.5 }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } }
		);
		// Back Face
		builder.CreateQuadFace(
			{ { -0.5f, -0.5f,  0.5 }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  0.5f, -0.5f,  0.5 }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  0.5f,  0.5f,  0.5 }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ { -0.5f,  0.5f,  0.5 }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
		);
		// Right Face
		builder.CreateQuadFace(
			{ {  0.5f,  0.5f, -0.5 }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ {  0.5f,  0.5f,  0.5 }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ {  0.5f, -0.5f,  0.5 }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
			{ {  0.5f, -0.5f, -0.5 }, { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } }
		);
		// Left Face
		builder.CreateQuadFace(
			{ { -0.5f,  0.5f,  0.5 }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { -0.5f,  0.5f, -0.5 }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { -0.5f, -0.5f, -0.5 }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { -0.5f, -0.5f,  0.5 }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } }
		);
		// Top Face
		builder.CreateQuadFace(
			{ { -0.5f, 0.5f,  0.5 }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
			{ {  0.5f, 0.5f,  0.5 }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
			{ {  0.5f, 0.5f, -0.5 }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f, -0.5 }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } }
		);
		// Bottom Face
		builder.CreateQuadFace(
			{ {  0.5f, -0.5f, -0.5 }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
			{ {  0.5f, -0.5f,  0.5 }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },
			{ { -0.5f, -0.5f,  0.5 }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },
			{ { -0.5f, -0.5f, -0.5 }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } }
		);

		return Mesh::Create("Cube", builder);
	}

	Ref<Mesh> Mesh::GetCubeMesh()
	{
		static Ref<Mesh> cubeMesh = GenerateCube();
		return cubeMesh;
	}

	Ref<Mesh> Mesh::GetPlaneMesh()
	{
		static Ref<Mesh> planeMesh = GeneratePlane();
		return planeMesh;
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

	MeshBuilder Mesh::CreateSphereMesh(uint32_t segments)
	{
		MeshBuilder builder(segments * segments, segments * segments * 6);

		auto vtop = builder.AddVertex({ 0.0f, 1.0f, 0.0f }, { 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		auto vbottom = builder.AddVertex({ 0.0f, -1.0f, 0.0f }, { 0.5f, 1.0f }, { 0.0f, -1.0f, 0.0f });

		// Go trough segments in horizontal direction.
		for (uint32_t i = 0; i < segments; i++)
		{
			// Go trough segments in vertical direction and skip the top and bottom vertices.
			for (uint32_t j = 1; j < segments - 1; j++)
			{
				float x = cosf((float)i / (float)segments * 2.0f * (float)M_PI) * sinf((float)j / (float)segments * (float)M_PI);
				float y = cosf((float)j / (float)segments * (float)M_PI);
				float z = sinf((float)i / (float)segments * 2.0f * (float)M_PI) * sinf((float)j / (float)segments * (float)M_PI);

				//auto v = builder.AddVertex({ x, y, z }, { (float)i / (float)segments, (float)j / (float)segments }, );
				auto v = builder.AddVertex({ x, y, z }, { (float)i / (float)segments, (float)j / (float)segments }, { x, y, z });
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
			builder.AddTriangle(vbottom, i * inner_segments + inner_segments + 1, (i + 1) * inner_segments + inner_segments + 1);
		}

		builder.AddTriangle(vtop, 2, (segments - 1) * inner_segments + 2);
		for (uint32_t j = 1; j < segments - 2; j++)
		{
			builder.AddQuad(j + 2, (segments - 1) * inner_segments + j + 2, (segments - 1) * inner_segments + j + 1, j + 1);
		}
		builder.AddTriangle(vbottom, (segments - 1) * inner_segments + inner_segments + 1, inner_segments + 1);

		return builder;
	}

	Ref<Mesh> Mesh::GetDynamicSphereMesh(uint32_t segments)
	{		
		return Mesh::Create("Sphere_" + std::to_string(segments), CreateSphereMesh(segments));
	}

	Ref<Mesh> Mesh::GetDynamicConeMesh(uint32_t segments)
	{
		MeshVertex* vertices = new MeshVertex[segments * 3 + 2];
		uint32_t* indices = new uint32_t[segments * 3 * 2];
		ZeroMemory(indices, sizeof(indices));

		vertices[0] = { { 0.0f,-1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f } };
		vertices[1] = { { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f,  1.0f, 0.0f } };

		for (uint32_t i = 0; i < segments; i++)
		{
			// Vertices 
			float angle = (float)i / (float)segments * 2.0f * 3.14159265359f;
			float x = cos(angle);
			float z = sin(angle);

			// Y = alpha
			// a = 0.5
			// b = 2.0
			// c = sqrt(pow(a, 2) + pow(b, 2))
			// c = sqrt(4.25)
			// c = 2.06155281281
			// alpha = asin(a / c)
			// alpha = asin(0.5 / 2.06155281281)
			static float alpha = 0.24497866312686414f;
			vertices[i + 2] = { { x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f }, { x, alpha , z } };
			vertices[i + 2 + segments] = { { x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f }, { 0.0f, -1.0f , 0.0f } };
			vertices[i + 2 + segments * 2] = { { 0.0f, 1.0f, 0.0f , }, { 0.5f, 0.5f }, { x, alpha , z } };
			
			// Indices
			indices[i * 3] = 0;
			indices[i * 3 + 1] = i + 2 + segments;
			indices[i * 3 + 2] = ((segments - 1) == i ? 2 : i + 3) + segments;

			indices[(i * 3 + segments * 3) + 2] = i + 2;
			indices[(i * 3 + segments * 3) + 1] = (segments - 1) == i ? 2 : i + 3;
			indices[(i * 3 + segments * 3)] = ((segments - 1) == i ? 2 : i + 3) + segments + segments;
		}

		return Mesh::Create("Cone_" + std::to_string(segments), vertices, segments * 3 + 2, indices, segments * 3 * 2);
	}

	Ref<Mesh> Mesh::GetDynamicCylinderMesh(uint32_t segments)
	{
		MeshBuilder builder;

		auto vtop = builder.AddVertex({ 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f });
		auto vbottom = builder.AddVertex({ 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f });

		for (uint32_t i = 0; i <= segments; i++)
		{
			float angle = (float)i / (float)segments * 2.0f * 3.14159265359f;
			float x = cos(angle);
			float z = sin(angle);

			auto v = builder.AddVertex({ x, -1.0f, z }, { angle / (PI * 2), 1.0f }, { x, 0.0f, z });
			auto v2 = builder.AddVertex({ x, 1.0f, z }, { angle / (PI * 2), 0.0f }, { x, 0.0f, z });

			auto vv = builder.AddVertex({ x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f }, { 0.0f, -1.0f, 0.0f });
			auto vv2 = builder.AddVertex({ x, 1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f }, { 0.0f, 1.0f, 0.0f });
			
			if(i > 0)
			{
				builder.AddQuad(v2 - 4, v2, v, v - 4);
				builder.AddTriangle(vtop, vv2, vv2 - 4);
				builder.AddTriangle(vbottom, vv - 4, vv);
			}
		}
		
		return Mesh::Create("Cylinder_" + std::to_string(segments), builder);
	}
	
	Mesh::Mesh(std::string name, const MeshVertex* vertices, uint32_t vertexCount, uint32_t* indices, uint32_t indexCount)
	{
		m_Vertices = std::vector<MeshVertex>(vertices, vertices + vertexCount);

		m_VertexArray = VertexArray::Create(GetInputLayout(),
			{ 
				{ UnsafeVertexBuffer::Create(vertices, vertexCount * sizeof(MeshVertex)) },
				{ GetInstanceVertexBuffer() }
			}
		);

		m_IndexBuffer = IndexBuffer::Create(indices, indexCount);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	Mesh::Mesh(std::string name, const MeshBuilder& meshBuilder)
	{
		m_Vertices = std::vector<MeshVertex>(meshBuilder.m_Vertices);
		
		m_VertexArray = VertexArray::Create(GetInputLayout(),
			{
				{ UnsafeVertexBuffer::Create(meshBuilder.m_Vertices.data(), meshBuilder.m_Vertices.size() * sizeof(MeshVertex)) },
				{ GetInstanceVertexBuffer() }
			}
		);

		m_IndexBuffer = IndexBuffer::Create((uint32_t*)meshBuilder.m_Triangles.data(), meshBuilder.m_Triangles.size() * 3);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
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

	Ref<Mesh> Mesh::GetTorusMesh(uint32_t segments_ring, uint32_t segments_cylinder, float R, float r)
	{
		std::string name = "Torus_" + std::to_string(segments_ring) + "_" + std::to_string(segments_cylinder) + "_" + std::to_string(R) + "_" + std::to_string(r);

		if (s_Meshes.find(name) != s_Meshes.end())
		{
			return s_Meshes[name];
		}
		else
		{
			Ref<Mesh> mesh = CreateRef<Mesh>(name, CreateTorus(segments_ring, segments_cylinder, R, r));
			s_Meshes[name] = mesh;
			return mesh;
		}
	}

	/*
	 * Creates a torus mesh with the given parameters.
	 * @param segments_ring The number of segments in the ring.
	 * @param segments_cylinder The number of segments in the cylinder.
	 * @param R The radius of the ring.
	 * @param r The radius of the cylinder.
	 * @return The mesh.
	*/
	MeshBuilder Mesh::CreateTorus(uint32_t segments_ring, uint32_t segments_cylinder, float R, float r)
	{
		MeshBuilder builder;

		for (int i = 0; i < segments_ring; i++)
		{
			float angle = (float)i / (float)segments_ring * 2.0f * PI;
			for (int j = 0; j < segments_cylinder; j++)
			{
				float angle2 = (float)j / (float)segments_cylinder * 2.0f * PI;
				float x = (R + r * cos(angle2)) * cos(angle);
				float y = r * sin(angle2);
				float z = (R + r * cos(angle2)) * sin(angle);
				builder.AddVertex({ Vector3(x, y, z), Vector2(angle / (PI * 2), angle2 / (PI * 2)), Vector3(cos(angle2) * cos(angle), sin(angle2), cos(angle2) * sin(angle)) });
			}
		}

		for (int i = 0; i < segments_ring; i++)
		{
			for (int j = 0; j < segments_cylinder; j++)
			{
				int i2 = (i + 1) % segments_ring;
				int j2 = (j + 1) % segments_cylinder;
				builder.AddQuad(i * segments_cylinder + j, i * segments_cylinder + j2, i2 * segments_cylinder + j2, i2 * segments_cylinder + j);
			}
		}
		return builder;
	}

	MeshBuilder Mesh::CreateGizmosArrow(uint32_t segments)
	{
		MeshBuilder arrowBuilder;
		
		const float arrowHeadHeight = 0.2f;
		const float arrowHeadRadius = 0.1f;
		const float arrowBodyHeight = 1.0f - arrowHeadHeight;
		const float arrowBodyRadius = 0.03f;

		auto arrowBottom = arrowBuilder.AddVertex({ Vector3(0.0f, 0.0f, 0.0f) });
		auto arrowHead = arrowBuilder.AddVertex({ Vector3(0.0f,  1.0f, 0.0f) });

		for (int i = 0; i < segments; i++)
		{
			float angle = (float)i / (float)segments * 2.0f * PI;

			auto arrowHeadBottom = arrowBuilder.AddVertex({ Vector3(arrowHeadRadius * cos(angle), arrowBodyHeight, arrowHeadRadius * sin(angle)) });
			auto arrowBodyTop = arrowBuilder.AddVertex({ Vector3(arrowBodyRadius * cos(angle), arrowBodyHeight, arrowBodyRadius * sin(angle)) });
			auto arrowBodyBottom = arrowBuilder.AddVertex({ Vector3(arrowBodyRadius * cos(angle), 0.0f, arrowBodyRadius * sin(angle)) });

			if (i == segments - 1)
			{
				arrowBuilder.AddTriangle(arrowHead, 2, arrowHeadBottom);
				arrowBuilder.AddQuad(arrowHeadBottom, 2, 3, arrowBodyTop);
				arrowBuilder.AddQuad(arrowBodyTop, 3, 4, arrowBodyBottom);
				arrowBuilder.AddTriangle(arrowBottom, arrowBodyBottom, 4);
			}
			else
			{
				arrowBuilder.AddTriangle(arrowHead, arrowHeadBottom + 3, arrowHeadBottom);
				arrowBuilder.AddQuad(arrowHeadBottom, arrowHeadBottom + 3, arrowBodyTop + 3, arrowBodyTop);
				arrowBuilder.AddQuad(arrowBodyTop, arrowBodyTop + 3, arrowBodyBottom + 3, arrowBodyBottom);
				arrowBuilder.AddTriangle(arrowBottom, arrowBodyBottom, arrowBodyBottom + 3);
			}
		}

		return arrowBuilder;
	}

	void Mesh::Bind()
	{
		m_VertexArray->Bind();
	}
}