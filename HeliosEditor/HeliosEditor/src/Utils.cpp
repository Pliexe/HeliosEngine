#include "Utils.h"

namespace Helios::Tools
{
    bool is_writeable(const std::filesystem::path &path)
    {
        std::error_code ec;
        std::filesystem::permissions(path, std::filesystem::perms::owner_write, ec);
        return !ec;
    }
}