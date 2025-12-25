#include "BundleAssets.h"
#include "Helios/Resources/Bundler/Bundle.h"
#include "Helios/Resources/ResourceResolver.h"

namespace Helios
{
    BuildStep::Action GetEditorBundleBuildAction(const std::filesystem::path& output_path)
    {
        return [output_path](BuildStep::ProgressCallback callback)
        {
            callback("Starting asset bundling", 0.0f);
            
            
            std::vector<BundleAsset> assets;   
            
            for (auto& resource : ResourceResolver::GetTextures())
            {
                if (resource.second.type != ResourceInfo::ResourceType::FULL_RESOURCE_AND_METADATA_FILE)
                {
                    continue;
                }

                BundleAsset asset = {
                    .uuid = resource.first,
                    .type = BundleResourceType::Texture,
                    .flags = BundleAsset::Flags::None,
                    .path = resource.second.path,
                    .metadata = nullptr,
                    .metadataSize = 0
                };

                assets.push_back(std::move(asset));
            }
            
            SerializeBundle(output_path, assets);

            callback("Finished asset bundling", 1.0f);
            return true;
        };
    }
}