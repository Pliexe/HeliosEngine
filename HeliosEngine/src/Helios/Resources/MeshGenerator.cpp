#include "MeshGenerator.h"
#include "ResourceResolver.h"

namespace Helios
{
	inline static std::unordered_map<MeshType, UUID> g_meshTypeToUuidMap;
	inline static std::unordered_map<UUID, MeshType> g_uuidToMehTypeMap;

	void MeshGenerator::InitializeMeshTypeMapping() {
          // Populate the mapping of mesh to UUID - uuid that begins from
          // 00000000-0000-0000-0000-000000000000
          g_meshTypeToUuidMap[MeshType::Plane] =
              UUID::fromString("00000000-0000-0000-0000-000000000000");
          g_meshTypeToUuidMap[MeshType::Cube] =
              UUID::fromString("00000000-0000-0000-0000-000000000001");
          g_meshTypeToUuidMap[MeshType::Sphere] =
              UUID::fromString("00000000-0000-0000-0000-000000000002");
          g_meshTypeToUuidMap[MeshType::Cylinder] =
              UUID::fromString("00000000-0000-0000-0000-000000000003");
          g_meshTypeToUuidMap[MeshType::Cone] =
              UUID::fromString("00000000-0000-0000-0000-000000000004");
          g_meshTypeToUuidMap[MeshType::Torus] =
              UUID::fromString("00000000-0000-0000-0000-000000000005");

          // Populate the mapping of UUID to MeshType
          for (const auto &pair : g_meshTypeToUuidMap) {
            g_uuidToMehTypeMap[pair.second] = pair.first;
            static ResourceInfo info = {
                ResourceInfo::INBUILT};
            ResourceResolver::RegisterResource(ResourceResolver::s_Meshes, pair.second, info);
		}
	}

	UUID MeshGenerator::GetMeshUUID(MeshType meshType)
	{ return g_meshTypeToUuidMap[meshType]; }

	MeshType MeshGenerator::GetMeshType(UUID uuid)
	{ return g_uuidToMehTypeMap[uuid]; }

	MeshBuilder MeshGenerator::GeneratePlane()
	{
		MeshBuilder builder;

		builder.CreateQuadFace(
			{ { -2.0f, 0.0f, -2.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { -2.0f, 0.0f, 2.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { 2.0f, 0.0f, 2.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { 2.0f, 0.0f, -2.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } }
		);

		return builder;
	}

	MeshBuilder MeshGenerator::GenerateCube()
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

		return builder;
	}

	MeshBuilder MeshGenerator::GenerateSphere(uint32_t segments)
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
				float x = cosf((float)i / (float)segments * 2.0f * (float)H_PI) * sinf((float)j / (float)segments * (float)H_PI);
				float y = cosf((float)j / (float)segments * (float)H_PI);
				float z = sinf((float)i / (float)segments * 2.0f * (float)H_PI) * sinf((float)j / (float)segments * (float)H_PI);

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

	MeshBuilder MeshGenerator::GenerateCylinder(uint32_t segments)
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

			if (i > 0)
			{
				builder.AddQuad(v2 - 4, v2, v, v - 4);
				builder.AddTriangle(vtop, vv2, vv2 - 4);
				builder.AddTriangle(vbottom, vv - 4, vv);
			}
		}
		
		return builder;
	}

	MeshBuilder MeshGenerator::GenerateCone(uint32_t segments)
	{
		MeshBuilder builder;

		builder.AddVertex({ 0.0f,-1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, -1.0f, 0.0f });
		builder.AddVertex({ 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f,  1.0f, 0.0f });

		// Y = alpha
		// a = 0.5
		// b = 2.0
		// c = sqrt(pow(a, 2) + pow(b, 2))
		// c = sqrt(4.25)
		// c = 2.06155281281
		// alpha = asin(a / c)
		// alpha = asin(0.5 / 2.06155281281)

		for (uint32_t i = 0; i < segments; i++)
		{
			float angle = (float)i / (float)segments * 2.0f * 3.14159265359f;
			float x = cos(angle);
			float z = sin(angle);

			static float alpha = 0.24497866312686414f;
			builder.AddVertex({ x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f }, { x, alpha , z });
			builder.AddVertex({ x, -1.0f, z }, { x * 0.5f + 0.5f, z * 0.5f + 0.5f }, { 0.0f, -1.0f , 0.0f });
			builder.AddVertex({ 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f }, { x, alpha , z });

			builder.AddTriangle(0, i + 2 + segments, ((segments - 1) == i ? 2 : i + 3) + segments);
			builder.AddTriangle(i + 2, (segments - 1) == i ? 2 : i + 3, ((segments - 1) == i ? 2 : i + 3) + segments + segments);
		}

		return builder;
	}

	MeshBuilder MeshGenerator::GenerateTorus(uint32_t segments_ring, uint32_t segments_cylinder, float R, float r)
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

	// ----------------------------------------------------------------------------

	MeshBuilder MeshGenerator::GenerateGizmosArrow(const uint32_t segments)
	{
		MeshBuilder arrowBuilder;

		constexpr float arrowHeadHeight = 0.2f;
		constexpr float arrowHeadRadius = 0.1f;
		constexpr float arrowBodyHeight = 1.0f - arrowHeadHeight;
		constexpr float arrowBodyRadius = 0.03f;

		auto arrowBottom = arrowBuilder.AddVertex({ Vector3(0.0f, 0.0f, 0.0f) });
		auto arrowHead = arrowBuilder.AddVertex({ Vector3(0.0f,  1.0f, 0.0f) });

		for (int i = 0; i < segments; i++)
		{
			float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * PI;

			const auto arrowHeadBottom = arrowBuilder.AddVertex({ Vector3(arrowHeadRadius * cos(angle), arrowBodyHeight, arrowHeadRadius * sin(angle)) });
			const auto arrowBodyTop = arrowBuilder.AddVertex({ Vector3(arrowBodyRadius * cos(angle), arrowBodyHeight, arrowBodyRadius * sin(angle)) });
			const auto arrowBodyBottom = arrowBuilder.AddVertex({ Vector3(arrowBodyRadius * cos(angle), 0.0f, arrowBodyRadius * sin(angle)) });

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


}