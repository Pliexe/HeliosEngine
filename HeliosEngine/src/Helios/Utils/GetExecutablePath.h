#pragma once

#include "pch.h"
#include "Helios/Core/Exceptions.h"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

namespace Helios
{
    inline std::filesystem::path GetExeDir()
    {
    #ifdef _WIN32
        char buffer[MAX_PATH];
        DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
        if (length == 0) HL_EXCEPTION(true, "GetModuleFileNameA failed");
        return std::filesystem::path(buffer).parent_path();

    #elif __APPLE__
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0) {
            HL_EXCEPTION(true, "Buffer too small for executable path");
        }
        return std::filesystem::canonical(buffer).parent_path();

    #elif __linux__
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (count == -1) HL_EXCEPTION(true, "readlink failed");
        return std::filesystem::path(std::string(buffer, count)).parent_path();

    #else
        HL_EXCEPTION(true, "Unsupported platform");
    #endif
    }

}