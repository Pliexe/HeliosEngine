#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

#include "Texture.h"
#include "Mesh.h"
#include "MeshGenerator.h"
#include <filesystem>

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

		static Ref<Mesh> ResolveMesh(const Helios::UUID& id);
		static Ref<Texture2D> ResolveTexture2D(const Helios::UUID& id);

		template <typename T>
		static Ref<T> Resolve(const Helios::UUID& id)
		{
			if constexpr (std::is_same_v<Texture2D, T>)
			{
				return ResolveTexture2D(id);
			}
			else if constexpr (std::is_same_v<Mesh, T>)
			{
				return ResolveMesh(id);
			}
		}

		static void RegisterResource(const std::filesystem::path& path) { RegisterResource(UUID(), path); }
		static void RegisterResource(const UUID& uuid, const std::filesystem::path& path)
		{
			if (!std::filesystem::exists(path))
			{
				throw std::runtime_error("File does not exist: " + path.string());
			}
			
			RegisterResource(uuid, ResourceInfo{ ResourceInfo::FULL_RESOURCE, path.string().c_str() });
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
				METADATA_FILE,
				BUNDLED_FILE
			};

			ResourceType type;
			const char* path = nullptr;
		};

		//inline static std::unordered_map<UUID, ResourceInfo> m_Textures;
		inline static std::unordered_map<UUID, ResourceInfo> s_Meshes;
		inline static std::unordered_map<UUID, ResourceInfo> s_Textures;

		friend class MeshGenerator;
	};
}