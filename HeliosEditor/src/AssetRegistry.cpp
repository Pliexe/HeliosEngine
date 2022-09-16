#include "AssetRegistry.h"

namespace Helios
{
	std::unordered_map<std::string, Ref<Texture2D>> AssetRegistry::s_Textures;
	bool AssetRegistry::s_CheckForChanges = false;
	bool AssetRegistry::s_RegistryWindowOpen = true;

	bool AssetRegistry::s_TextureSelect = false;
	std::function<void(Ref<Texture2D>)> AssetRegistry::s_onTextureSelected;
}