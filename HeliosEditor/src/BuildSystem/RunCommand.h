#pragma once

#include "pch.h"
#include <Helios/Utils/Conversions.h>

namespace Helios
{
    struct CommandResult
    {
        int exitCode;
        std::string output;
    };

    CommandResult RunCommand(const std::u8string& command, std::function<void(const std::string&)> onOutput);

    CommandResult RunCommand(const std::u8string& command);
}