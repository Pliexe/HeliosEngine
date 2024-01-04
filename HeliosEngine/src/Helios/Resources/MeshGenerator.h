#pragma once

#include "pch.h"
#include "Helios/Core/UUID.h"
#include "Mesh.h"

namespace Helios
{
	enum class MeshType
	{
		Plane,
		Cube,
		Sphere,
		Cylinder,
		Cone,
		Torus,
	};

	class HELIOS_API MeshGenerator
	{
	private:
		inline static std::unordered_map<MeshType, UUID> meshTypeToUuuidMap;
		inline static std::unordered_map<UUID, MeshType> uuidToMehTypeMap;

		static void InitalizeMeshTypeMapping();
	public:

		static UUID GetMeshUUID(MeshType meshType) { return meshTypeToUuuidMap[meshType]; }
		static MeshType GetMeshType(UUID uuid) { return uuidToMehTypeMap[uuid]; }

		static MeshBuilder GenerateMesh(UUID meshUUID)
		{
			switch (GetMeshType(meshUUID))
			{
			case MeshType::Plane:
				return MeshGenerator::GeneratePlane();
			case MeshType::Cube:
				return MeshGenerator::GenerateCube();
			case MeshType::Sphere:
				return MeshGenerator::GenerateSphere(32);
			case MeshType::Cylinder:
				return MeshGenerator::GenerateCylinder(32);
			case MeshType::Cone:
				return MeshGenerator::GenerateCone(32);
			case MeshType::Torus:
				return MeshGenerator::GenerateTorus(32, 32, 0.5f, 0.25f);
			default:
				return MeshBuilder();
			}
		}

		// GENERALE USE
		static MeshBuilder GeneratePlane();
		static MeshBuilder GenerateCube();
		static MeshBuilder GenerateSphere(uint32_t segments);
		static MeshBuilder GenerateCylinder(uint32_t segments);
		static MeshBuilder GenerateCone(uint32_t segments);
		static MeshBuilder GenerateTorus(uint32_t segments_ring, uint32_t segments_cylinder, float R, float r);

		// FOR EDITOR
		static MeshBuilder GenerateGizmosArrow(uint32_t segments);

		friend class Application;
	};
}