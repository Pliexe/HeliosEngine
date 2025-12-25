#pragma once
#include "../../../../TestApp/src/UnitTests/UUID.h"
#include "Helios/Resources/Material.h"
#include "Helios/Resources/Mesh.h"

namespace Helios
{
	class AssetManager
	{
	private:

		struct TextureData
		{
			std::string path;
			std::string meta_path;
			struct
			{
				uint32_t resolution;
			} meta = { 2048u };

			bool operator==(const TextureData& other) const
			{
				return path == other.path;
			}

			bool operator!=(const TextureData& other) const
			{
				return !(*this == other);
			}
		};

		inline static std::unordered_map<UUID, Ref<MaterialInstance>> s_Materials;
		inline static std::unordered_map<UUID, Ref<Mesh>> s_Meshes;
		inline static std::unordered_map<UUID, Ref<Texture2D>> s_Textures;
		// inline static std::unordered_map<UUID, Ref<DepricatedShader>> s_Shaders;

		inline static std::unordered_map<UUID, TextureData> s_TextureData;

	public:

		template <typename T>
		std::enable_if_t<std::is_same_v<T, Texture2D>, const std::unordered_map<UUID, TextureData>&>
		static GetAll()
		{
			return s_TextureData;
		}

		static void RegisterTexture(const std::string& texture_path, const std::string& meta_path)
		{
			TextureData data;
			data.path = texture_path;

			if (!std::filesystem::exists(meta_path))
				CreateMeta<Texture2D>(meta_path);

			std::ifstream in(meta_path);
			json meta;
			in >> meta;
			in.close();

			data.meta.resolution = meta["Resolution"];
			data.meta_path = meta_path;

			s_TextureData[UUID::fromString(meta["UUID"].get<std::string>())] = data;
		}

		template <typename T>
		std::enable_if_t<std::is_same_v<T, Texture2D>, void>
		static CreateMeta(std::filesystem::path path)
		{
			json meta;
			meta["UUID"] = UUID().toString();
			meta["Resolution"] = 2048.0f;

			std::ofstream out(path);
			out << meta.dump(4);
			out.close();
		}

		static Ref<Texture2D> GetTexture(UUID uuid)
		{
			if (s_Textures.find(uuid) != s_Textures.end())
				return s_Textures[uuid];

			auto& data = s_TextureData[uuid];
			auto texture = Texture2D::Create(data.path);
			s_Textures[uuid] = texture;
			return texture;
		}


		static UUID GetUUID(Ref<Texture2D> _texture)
		{
			for (auto& [uuid, texture] : s_Textures)
			{
				if (texture == _texture)
					return uuid;
			}
			return UUID();
		}

		static void OnUpdate()
		{
			//// Unload unused assets
			//for (auto it = s_Textures.begin(); it != s_Textures.end();)
			//{
			//	if (it->second.use_count() == 1)
			//	{
			//		it = s_Textures.erase(it);
			//	}
			//	else
			//	{
			//		it++;
			//	}
			//}
		}
	};
}
