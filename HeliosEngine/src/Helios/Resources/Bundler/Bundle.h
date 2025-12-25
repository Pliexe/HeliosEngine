#pragma once

#include "pch.h"

#include "Helios/Core/Base.h"
#include "Helios/Core/UUID.h"

#define HELIOS_ASSET_BUNDLE_SIGNATURE "HELIOS_ASSET_BUNDLE"
#define HELIOS_BUNDLER_VERSION HELIOS_VERSION_TO_NUMBER(0, 1, 0)
#define HELIOS_BUNDLER_VERSION_STRING "0.1.0"

namespace Helios
{
    enum class BundleResourceType : uint32_t
    {
        Texture = 1,
        Mesh = 2,
        Shader = 3,
        Material = 4,
        Prefab = 5,
        PrefabInstance = 6,
    };
    
    struct BundleRegistryEntry
    {
        UUID ID; // Unique identifier for the resource
        uint64_t Version; // Version of the resource
        uint32_t Flags; // Flags for the resource (e.g., compressed, encrypted, etc.)

        // Resource entry metadata
        BundleResourceType Type; // Type of the resource (e.g., texture, mesh, etc.)
        uint64_t Offset; // Offset in the bundle file where the resource data starts
        uint64_t Size; // Size of the resource data
        uint64_t Hash; // Hash of the resource data for integrity checking
        // Additional metadata after resource data
        uint64_t MetadataOffset; // Offset in the bundle file where the metadata starts
        uint32_t MetadataSize; // Size of the metadata
        uint32_t MetadataHash; // Hash of the metadata for integrity checking
    };
    
    struct BundleFile
    {
        char EngineSignature[11]; // = "HELIOS_FILE"
        uint64_t EngineVersion; // = HELIOS_ENGINE_VERSION
        char FileSignature[19]; // = HELIOS_ASSET_BUNDLE
        uint64_t FileVersion; // = HELIOS_BUNDLER_VERSION
        
        uint32_t FileCount; // Number of files in the bundle

        // Array of BundleRegistryEntry for each file in the bundle

        // The actual data for the files would follow this structure in the file
    };

    int32_t ValidateBundle(const std::filesystem::path& path, uint64_t min_version = 0, uint64_t max_version = 0);

    const std::vector<UUID> GetBundleUUIDs(const std::filesystem::path& path, uint64_t min_version = 0, uint64_t max_version = 0);
    const std::vector<std::byte> GetBundleData(const std::filesystem::path& path, const UUID& uuid, uint64_t min_version = 0, uint64_t max_version = 0);
    const std::vector<std::byte> GetBundleMetadataRaw(const std::filesystem::path& path, const UUID& uuid, uint64_t min_version = 0, uint64_t max_version = 0);

    template <typename T>
    T GetBundleMetadata(const std::filesystem::path& path, const Helios::UUID& uuid, uint64_t min_version = 0, uint64_t max_version = 0)
    {
        auto data = GetBundleMetadataRaw(path, uuid, min_version, max_version);
        T metadata;
        if (data.size() < sizeof(T))
        {
            throw std::runtime_error("Invalid metadata size for UUID: " + uuid.toString());
        }
        std::memset(&metadata, 0, sizeof(T));
        std::memcpy(&metadata, data.data(), sizeof(T));
        return metadata;
    }

    struct BundleAsset {
        enum class Flags : bitmask_t
        {
            None = 0,
            Raw = BIT(0),
            Compress = BIT(1),
            Encrypt = BIT(2),
        };
        
        UUID uuid;
        BundleResourceType type;
        Flags flags;    
        std::filesystem::path path; // Path to the resource file
        std::vector<std::byte> rawData; // Raw data for the resource
        const void* metadata; // Pointer to the metadata for the resource
        size_t metadataSize; // Size of the metadata
    };
    
    constexpr BundleAsset::Flags operator|(BundleAsset::Flags lhs, BundleAsset::Flags rhs) {
        return lhs | rhs;
    }

    constexpr BundleAsset::Flags operator|=(BundleAsset::Flags lhs, BundleAsset::Flags rhs) {
        lhs = lhs | rhs;
        return lhs;
    }
    
    constexpr BundleAsset::Flags operator&(BundleAsset::Flags lhs, BundleAsset::Flags rhs) {
        return lhs & rhs;
    }
    
    constexpr BundleAsset::Flags operator^(BundleAsset::Flags lhs, BundleAsset::Flags rhs) {
        return lhs ^ rhs;
    }
    
    constexpr BundleAsset::Flags operator~(BundleAsset::Flags f) {
        return ~f;
    }

    int32_t SerializeBundle(const std::filesystem::path& path, const std::vector<BundleAsset>& assets, uint64_t min_version = 0, uint64_t max_version = 0);
}