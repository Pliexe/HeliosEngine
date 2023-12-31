#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

#include "MeshGenerator.h"
#include "Texture.h"
#include "Mesh.h"

namespace Helios
{
	// Manages all loaded resources
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
		}

		template <typename T>
		static Ref<T> GetResource(MeshType type)
		{
			return GetResource<T>(MeshGenerator::GetMeshUUID(type));
		}

	private:

		inline static std::unordered_map<UUID, Ref<Texture2D>> s_Textures;
		inline static std::unordered_map<UUID, Ref<Mesh>> s_Meshes;
	};
}