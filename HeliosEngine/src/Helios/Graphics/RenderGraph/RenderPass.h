#pragma once

#include "Helios/Graphics/RenderGraph/RenderContext.h"
#include "Helios/Resources/Image.h"
#include "pch.h"
#include "Helios/Math/Matrix.h"

namespace Helios
{
    using RenderPassCallback = std::function<void(RenderContext& context)>;
    
    struct RenderPass
    {
        std::string name;
        RenderPassCallback callback;
    };
}