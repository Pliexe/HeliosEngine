#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

#include "Texture.h"
#include "Mesh.h"
#include "MeshGenerator.h"

namespace Helios
{
	// Manager for resolving resources from the filesystem or any other source
	class HELIOS_API ResourceResolver
	{
	private:
		struct ResourceInfo;
	public:

		/*template <typename T>
		static std::enable_if_t<std::is_base_of<Texture2D, T>::value, Ref<Texture2D>>
			Resolve(const Helios::UUID& id)
		{

		}*/

		template <typename T>
		static std::enable_if_t<std::is_base_of<Mesh, T>::value, Ref<Mesh>>
			Resolve(const Helios::UUID& id)
		{
			auto it = s_Meshes.find(id);

			if (it == s_Meshes.end())
				return nullptr;

			switch (it->second.type)
			{
			case ResourceInfo::INBUILT:
				return Mesh::Create("Mesh_" + id.toString(), id, MeshGenerator::GenerateMesh(id));
			case ResourceInfo::FULL_RESOURCE:
				break;
			case ResourceInfo::BUNDLED_FILE:
				break;
			}

			HL_ASSERT(false, "Not implemented");

			return nullptr;
		}

		static void RegisterResource(const std::filesystem::path& path) { RegisterResource(UUID(), path); }
		static void RegisterResource(const UUID& uuid, const std::filesystem::path& path)
		{
			static_assert("Not implemented");
		}

		

		static bool UnregisterResource(const UUID& uuid)
		{
			return s_Meshes.erase(uuid) > 0;
		}

	private:

		static void RegisterResource(const UUID& uuid, const ResourceInfo& info)
		{
			s_Meshes[uuid] = info;
		}

		struct ResourceInfo {
			enum ResourceType {
				INBUILT,
				FULL_RESOURCE,
				BUNDLED_FILE
			};

			ResourceType type;
			const char* path = nullptr;
		};

		//inline static std::unordered_map<UUID, ResourceInfo> m_Textures;
		inline static std::unordered_map<UUID, ResourceInfo> s_Meshes;

		friend class MeshGenerator;
	};
}