#pragma once

#include "pch.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

#include "Texture.h"
#include "Mesh.h"
#include "MeshGenerator.h"
#include <filesystem>

namespace Helios
{
	enum LoadOptions {
		None = 0,
		LoadFullResource = 1 << 0,
		LoadMetadataFile = 1 << 1,
		LoadBundledFile = 1 << 2,
		LoadAsOneMesh = 1 << 3
	};

	inline LoadOptions operator|(LoadOptions a, LoadOptions b)
	{
		return static_cast<LoadOptions>(static_cast<int>(a) | static_cast<int>(b));
	}

	// Manager for resolving resources from the filesystem or any other source

	struct ResourceInfo {
		enum ResourceType {
			INBUILT,
			FULL_RESOURCE,
			FULL_RESOURCE_AND_METADATA_FILE,
			BUNDLED_FILE
		};

		ResourceType type;
		const char8_t* path = nullptr;
	};

	class HELIOS_API ResourceResolver
	{
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

		static const UUID RegisterResource(const std::filesystem::path& path, LoadOptions loadOptions = LoadFullResource);

		static void RegisterResource(const UUID& uuid, const std::filesystem::path& path, LoadOptions loadOptions = LoadFullResource);

		static bool UnregisterResource(const UUID& uuid)
		{
			return false;
		}

		template <typename T>
		static bool IsUUIDRegistered(const Helios::UUID& uuid)
		{
			if constexpr (std::is_same_v<Texture2D, T>)
			{
				return s_Textures.contains(uuid);
			}
			else if constexpr (std::is_same_v<Mesh, T>)
			{
				return s_Meshes.contains(uuid);
			}
			else return false;
		}

		static bool IsPathRegistered(const std::filesystem::path& path)
		{
			return s_RegisteredPaths.contains(path);
		}

		static const UUID GetUUID(const std::filesystem::path& path);

		static std::unordered_map<UUID, Helios::ResourceInfo>& GetMeshes() { return s_Meshes; }
		static std::unordered_map<UUID, Helios::ResourceInfo>& GetTextures() { return s_Textures; }


	private:

		static void RegisterResource(std::unordered_map<UUID, ResourceInfo>& store, const UUID& uuid, const Helios::ResourceInfo& info)
		{
			store[uuid] = info;
			if (info.path) s_RegisteredPaths.insert(std::filesystem::path(info.path));
		}

		//inline static std::unordered_map<UUID, ResourceInfo> m_Textures;
		static std::unordered_map<UUID, Helios::ResourceInfo> s_Meshes;
		static std::unordered_map<UUID, Helios::ResourceInfo> s_Textures;

		static std::unordered_set<std::filesystem::path> s_RegisteredPaths;

		friend class MeshGenerator;
	};
}
