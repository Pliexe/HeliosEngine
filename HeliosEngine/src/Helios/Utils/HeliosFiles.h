#pragma once

#include "pch.h"

namespace Helios::File
{
    enum ReturnCodes : int32_t
    {
        Success = 0,
        FileNotFound = -1,
        InvalidFile = -2,
        CannotOpenFile = -3,
        FileOutdated = -4,
        FileTooNew = -5,
    };

    // Returns success if the file is valid, otherwise returns an error code.
    int32_t IsValid(const std::filesystem::path& path, const std::string& signature, uint64_t min_version = 0, uint64_t max_version = 0);
}