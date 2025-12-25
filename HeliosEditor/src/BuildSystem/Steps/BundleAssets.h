#pragma once

#include "../BuildTask.h"

namespace Helios
{
    BuildStep::Action GetEditorBundleBuildAction(const std::filesystem::path& output_path);
}