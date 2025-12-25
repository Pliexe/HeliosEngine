#include "HeliosFiles.h"
#include "Helios/Core/Base.h"

using namespace Helios;

int32_t File::IsValid(const std::filesystem::path &path, const std::string &signature, uint64_t min_version, uint64_t max_version)
{
    if (!std::filesystem::exists(path)) return ReturnCodes::FileNotFound; // File does not exist

    // Assuming that char is 1 byte
    size_t minSize = 11 + sizeof(uint64_t) + signature.length() + sizeof(uint64_t);
    if (std::filesystem::file_size(path) < minSize) {
        return ReturnCodes::InvalidFile; // File too small (invalid file)
    }
    
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return ReturnCodes::CannotOpenFile;
    }

    char engineSig[11];
    file.read(engineSig, 11);
    if (strcmp(engineSig, HELIOS_FILE_SIGNATURE) != 0) {
        return ReturnCodes::InvalidFile; // Invalid engine signature
    }

    uint64_t engineVersion;
    file.read(reinterpret_cast<char*>(&engineVersion), sizeof(uint64_t));

    std::vector<char> fileSig(signature.length() + 1);
    file.read(fileSig.data(), signature.length());
    fileSig[signature.length()] = '\0';
    if (strcmp(fileSig.data(), signature.c_str()) != 0) {
        return ReturnCodes::InvalidFile; // Invalid file signature
    }

    uint64_t fileVersion;
    file.read(reinterpret_cast<char*>(&fileVersion), sizeof(uint64_t));
    
    if (min_version != 0 && fileVersion < min_version) {
        return ReturnCodes::FileOutdated; // File version is too old
    }
    if (max_version != 0 && fileVersion > max_version) {
        return ReturnCodes::FileTooNew; // File version is too new
    }

    return ReturnCodes::Success; // File is valid
}