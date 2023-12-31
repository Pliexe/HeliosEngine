#include "ResourceRegistry.h"
#include "ResourceResolver.h"

namespace Helios
{
	Ref<Texture2D> ResourceRegistry::GetTextureResource(const Helios::UUID& id)
	{
		return Ref<Texture2D>();
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
			s_Meshes[id] = tmp;
			return tmp;
		}
	}
}