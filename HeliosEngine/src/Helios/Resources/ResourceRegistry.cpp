#include "ResourceRegistry.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Graphics/GraphicsPipeline.h"
#include "Helios/Resources/Shader.h"
#include "Platform/Vulkan/GraphicsPipeline/VkGraphicsPipeline.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "ResourceResolver.h"
#include "yaml-cpp/node/node.h"
#include <memory>
#include <mutex>
#include <optional>

namespace Helios
{
    std::unordered_map<UUID, Ref<Texture2D>> ResourceRegistry::s_Textures;
    std::unordered_map<UUID, Ref<Mesh>> ResourceRegistry::s_Meshes;
    std::unordered_map<UUID, Ref<EntityContainer>> ResourceRegistry::s_EntityContainers;
	std::filesystem::path ResourceRegistry::s_resourcePath;

	std::mutex ResourceRegistry::s_MutexGP;

	Ref<Texture2D> ResourceRegistry::GetTextureResource(const Helios::UUID& id)
	{
		if (s_Textures.find(id) != s_Textures.end())
		{
			return s_Textures[id];
		}
		else
		{
			Ref<Texture2D> tmp = ResourceResolver::Resolve<Texture2D>(id);
			if (tmp != nullptr) s_Textures[id] = tmp;
			return tmp;
		}
	}
	Ref<Mesh> ResourceRegistry::GetMeshResource(const Helios::UUID& id)
	{
		if (s_Meshes.find(id) != s_Meshes.end())
		{
			return s_Meshes[id];
		}
		else
		{
			Ref<Mesh> tmp = ResourceResolver::Resolve<Mesh>(id);
			if (tmp != nullptr) s_Meshes[id] = tmp;
			return tmp;
		}
	}
}