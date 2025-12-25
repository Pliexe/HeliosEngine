#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
    struct HELIOS_API ClearDepthStencil
    {
        float       depth;
        uint32_t    stencil;  
    };
}