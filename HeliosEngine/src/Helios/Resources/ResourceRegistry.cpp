#include "ResourceRegistry.h"
#include "ResourceResolver.h"

namespace Helios
{

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