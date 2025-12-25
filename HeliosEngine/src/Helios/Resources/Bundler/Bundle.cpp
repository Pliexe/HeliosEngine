#include "Bundle.h"
#include "Helios/Utils/HeliosFiles.h"
#include "BundleExceptions.h"
#include "Helios/Core/Base.h"

using namespace Helios;

int32_t Helios::ValidateBundle(const std::filesystem::path &path, uint64_t min_version, uint64_t max_version)
{
    int32_t result = File::IsValid(path, HELIOS_ASSET_BUNDLE_SIGNATURE, min_version, max_version);
    if (result != File::ReturnCodes::Success)
    {
        return result;
    }

    size_t minSize = 11 + sizeof(uint64_t) + std::strlen(HELIOS_ASSET_BUNDLE_SIGNATURE) + sizeof(uint64_t);
    
    BundleFile bundleFile = {};
    std::memset(&bundleFile, 0, sizeof(BundleFile));

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return File::ReturnCodes::CannotOpenFile;
    }
    file.read(reinterpret_cast<char*>(&bundleFile), sizeof(BundleFile));
    file.close();

    minSize += sizeof(BundleRegistryEntry) * bundleFile.FileCount;
    if (std::filesystem::file_size(path) < minSize)
    {
        return File::ReturnCodes::InvalidFile;
    }

    // Validate each entry for exceeding file size
    std::ifstream validateFile(path, std::ios::binary);
    validateFile.seekg(sizeof(BundleFile), std::ios::beg);

    auto fileSize = std::filesystem::file_size(path);
    
    for (uint32_t i = 0; i < bundleFile.FileCount; i++)
    {
        BundleRegistryEntry entry = {};
        std::memset(&entry, 0, sizeof(BundleRegistryEntry));
        validateFile.read(reinterpret_cast<char*>(&entry), sizeof(BundleRegistryEntry));

        if (entry.MetadataOffset + entry.MetadataSize > fileSize)
        {
            validateFile.close();
            return File::ReturnCodes::InvalidFile;
        }
    }
    validateFile.close();

    return File::ReturnCodes::Success;
}

const std::vector<Helios::UUID> Helios::GetBundleUUIDs(const std::filesystem::path &path, uint64_t min_version, uint64_t max_version)
{    
    int32_t result = ValidateBundle(path, min_version, max_version);
    
    switch (result)
    {
    case File::ReturnCodes::FileNotFound:
        throw BundleFileException("Bundle file not found: " + path.string());
    case File::ReturnCodes::InvalidFile:
        throw BundleBadFileException("Invalid bundle file: " + path.string());
    case File::ReturnCodes::CannotOpenFile:
        throw BundleFileException("Cannot open bundle file: " + path.string());
    case File::ReturnCodes::FileOutdated:
        throw BundleFileException("Bundle file is outdated: " + path.string());
    case File::ReturnCodes::FileTooNew:
        throw BundleFileException("Bundle file is too new: " + path.string());
    default:
        break;
    }

    std::vector<UUID> uuids;
    
    BundleFile bundleFile = {};
    std::memset(&bundleFile, 0, sizeof(BundleFile));

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        throw BundleFileException("Cannot open bundle file: " + path.string());
    }

    file.read(reinterpret_cast<char*>(&bundleFile), sizeof(BundleFile));
    
    for (uint32_t i = 0; i < bundleFile.FileCount; i++)
    {
        BundleRegistryEntry entry = {};
        std::memset(&entry, 0, sizeof(BundleRegistryEntry));
        file.read(reinterpret_cast<char*>(&entry), sizeof(BundleRegistryEntry));

        uuids.push_back(entry.ID);
    }
    file.close();

    return uuids;
}

const std::vector<std::byte> Helios::GetBundleData(const std::filesystem::path &path, const UUID &uuid, uint64_t min_version, uint64_t max_version)
{
    int32_t result = ValidateBundle(path, min_version, max_version);
    
    switch (result)
    {
    case File::ReturnCodes::FileNotFound:
        throw BundleFileException("Bundle file not found: " + path.string());
    case File::ReturnCodes::InvalidFile:
        throw BundleBadFileException("Invalid bundle file: " + path.string());
    case File::ReturnCodes::CannotOpenFile:
        throw BundleFileException("Cannot open bundle file: " + path.string());
    case File::ReturnCodes::FileOutdated:
        throw BundleFileException("Bundle file is outdated: " + path.string());
    case File::ReturnCodes::FileTooNew:
        throw BundleFileException("Bundle file is too new: " + path.string());
    default:
        break;
    }

    std::vector<std::byte> bytes;
    
    BundleFile bundleFile = {};
    std::memset(&bundleFile, 0, sizeof(BundleFile));

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        throw BundleFileException("Cannot open bundle file: " + path.string());
    }

    file.read(reinterpret_cast<char*>(&bundleFile), sizeof(BundleFile));
    
    for (uint32_t i = 0; i < bundleFile.FileCount; i++)
    {
        BundleRegistryEntry entry = {};
        std::memset(&entry, 0, sizeof(BundleRegistryEntry));
        file.read(reinterpret_cast<char*>(&entry), sizeof(BundleRegistryEntry));

        if (entry.ID == uuid)
        {
            bytes.resize(entry.Size);
            file.seekg(entry.Offset, std::ios::beg);
            file.read(reinterpret_cast<char*>(bytes.data()), entry.Size);
            break;
        }
    }
    file.close();

    return bytes;
}

const std::vector<std::byte> Helios::GetBundleMetadataRaw(const std::filesystem::path &path, const UUID &uuid, uint64_t min_version, uint64_t max_version)
{
    int32_t result = ValidateBundle(path, min_version, max_version);
    
    switch (result)
    {
    case File::ReturnCodes::FileNotFound:
        throw BundleFileException("Bundle file not found: " + path.string());
    case File::ReturnCodes::InvalidFile:
        throw BundleBadFileException("Invalid bundle file: " + path.string());
    case File::ReturnCodes::CannotOpenFile:
        throw BundleFileException("Cannot open bundle file: " + path.string());
    case File::ReturnCodes::FileOutdated:
        throw BundleFileException("Bundle file is outdated: " + path.string());
    case File::ReturnCodes::FileTooNew:
        throw BundleFileException("Bundle file is too new: " + path.string());
    default:
        break;
    }

    std::vector<std::byte> bytes;
    
    BundleFile bundleFile = {};
    std::memset(&bundleFile, 0, sizeof(BundleFile));

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        throw BundleFileException("Cannot open bundle file: " + path.string());
    }

    file.read(reinterpret_cast<char*>(&bundleFile), sizeof(BundleFile));
    
    for (uint32_t i = 0; i < bundleFile.FileCount; i++)
    {
        BundleRegistryEntry entry = {};
        std::memset(&entry, 0, sizeof(BundleRegistryEntry));
        file.read(reinterpret_cast<char*>(&entry), sizeof(BundleRegistryEntry));

        if (entry.ID == uuid)
        {
            bytes.resize(entry.MetadataSize);
            file.seekg(entry.MetadataOffset, std::ios::beg);
            file.read(reinterpret_cast<char*>(bytes.data()), entry.MetadataSize);
            break;
        }
    }
    file.close();

    return bytes;
}

int32_t Helios::SerializeBundle(const std::filesystem::path &path, const std::vector<BundleAsset> &assets, uint64_t min_version, uint64_t max_version)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        return File::ReturnCodes::CannotOpenFile;
    }

    BundleFile bundleFile = {};
    std::memset(&bundleFile, 0, sizeof(BundleFile));
    std::strcpy(bundleFile.EngineSignature, HELIOS_FILE_SIGNATURE);
    bundleFile.EngineVersion = HELIOS_ENGINE_VERSION;
    std::strcpy(bundleFile.FileSignature, HELIOS_ASSET_BUNDLE_SIGNATURE);
    bundleFile.FileVersion = HELIOS_BUNDLER_VERSION;
    bundleFile.FileCount = assets.size();
    
    file.write(reinterpret_cast<char*>(&bundleFile), sizeof(BundleFile));
    file.seekp(sizeof(BundleFile), std::ios::beg);

    std::vector<BundleRegistryEntry> entries(assets.size());
    std::memset(entries.data(), 0, sizeof(BundleRegistryEntry) * assets.size());
    uint64_t totalOffset = sizeof(BundleFile) + sizeof(BundleRegistryEntry) * assets.size();
    for (uint32_t i = 0; i < assets.size(); i++)
    {
        auto& entry = entries[i];
        std::memset(&entry, 0, sizeof(BundleRegistryEntry));
        entry.ID = assets[i].uuid;
        entry.Version = HELIOS_VERSION_TO_NUMBER(0,1,0);
        entry.Flags = 0;
        entry.Type = assets[i].type;
        
        if ((assets[i].flags & BundleAsset::Flags::Raw) != BundleAsset::Flags::None) // why cant I just flag & flag what the hell c++ me nobos what hehe shamon
        {
            entry.Offset = totalOffset;
            entry.Size = assets[i].rawData.size();
            entry.MetadataOffset = entry.Offset + entry.Size;
            entry.MetadataSize = assets[i].metadataSize;           
        }
        else
        {
            uint64_t fileSize = std::filesystem::file_size(assets[i].path);
            entry.Offset = totalOffset;
            entry.Size = fileSize;
            entry.MetadataOffset = entry.Offset + entry.Size;
            entry.MetadataSize = assets[i].metadataSize;
        }

        totalOffset += entry.Size + entry.MetadataSize;
    }

    file.seekp(sizeof(BundleFile), std::ios::beg);
    file.write(reinterpret_cast<char*>(entries.data()), sizeof(BundleRegistryEntry) * entries.size());

    for (uint32_t i = 0; i < assets.size(); i++)
    {
        auto& entry = entries[i];
        if ((assets[i].flags & BundleAsset::Flags::Raw) != BundleAsset::Flags::None)
        {
            file.write(reinterpret_cast<const char*>(assets[i].rawData.data()), assets[i].rawData.size());
        }
        else
        {
            std::ifstream assetFile(assets[i].path, std::ios::binary);
            if (!assetFile.is_open())
            {
                return File::ReturnCodes::CannotOpenFile;
            }
            std::copy(std::istreambuf_iterator<char>(assetFile),
                    std::istreambuf_iterator<char>(),
                    std::ostreambuf_iterator<char>(file));

        }
        file.write(reinterpret_cast<const char*>(assets[i].metadata), assets[i].metadataSize);
    }

    file.close();

    return File::ReturnCodes::Success;
}
