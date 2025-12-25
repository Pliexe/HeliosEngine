#pragma once

#include "Helios/Core/Logger.h"
#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

#include "MeshGenerator.h"
namespace Helios
{
	// Manages all loaded resources
	class Texture2D;
	class Mesh;
	class EntityContainer;
	class HELIOS_API ResourceRegistry
	{
	public:

		static Ref<Texture2D> GetTextureResource(const Helios::UUID& id);
		static Ref<Mesh> GetMeshResource(const Helios::UUID& id);

		template <typename T>
		static Ref<T> GetResource(const Helios::UUID& id)
		{
			if constexpr (std::is_same_v<Texture2D, T>)
			{
				return GetTextureResource(id);
			}
			else if constexpr (std::is_same_v<Mesh, T>)
			{
				return GetMeshResource(id);
			}
			else
			{
				static_assert(false, "Resource type not supported");
			}
		}

		template <typename T>
		static Ref<T> GetResource(MeshType type)
		{
			return GetResource<T>(MeshGenerator::GetMeshUUID(type));
		}
		

		template <typename T>
		static const std::unordered_map<UUID, Ref<T>>& GetResources()
		{
			if constexpr (std::is_same_v<Texture2D, T>)
			{
				return s_Textures;
			}
			else if constexpr (std::is_same_v<Mesh, T>)
			{
				return s_Meshes;
			}
			else
			{
				static_assert(false, "Resource type not supported");
			}
		}

		static void SetResourcePath(const std::filesystem::path& path)
		{
			if (std::filesystem::exists(path)) ResourceRegistry::s_resourcePath = path;
			else {
				HL_MESSAGE("Unknown Resource Path");
			}
		}

		static const std::filesystem::path& GetResourcePath() { return ResourceRegistry::s_resourcePath; }

	private:

		friend class Application;

		// Clear out all cached resources
		// This is called when the application is shutting down
		static void Shutdown()
		{
			s_Textures.clear();
			s_Meshes.clear();
			s_EntityContainers.clear();
		}
		
		static std::unordered_map<UUID, Ref<Texture2D>> s_Textures;
		static std::unordered_map<UUID, Ref<Mesh>> s_Meshes;
		static std::unordered_map<UUID, Ref<EntityContainer>> s_EntityContainers;

		static std::filesystem::path s_resourcePath;

		static std::mutex s_MutexGP;
	};
}