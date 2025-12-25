#pragma once

#include "Helios/Resources/ClearDepthStencil.h"

#include "Helios/Resources/Color.h"
#include "Helios/Resources/Image.h"

namespace Helios
{

    struct RenderPassBeginInfo
    {
        enum class LoadOp
        {
            Clear,
            Load,
            DontCare
        };

        enum class StoreOp
        {
            DontCare,
            Store
        };

        struct AttachmentInfo
        {
            Ref<Image> buffer;
            LoadOp loadOp = LoadOp::Clear;
            StoreOp storeOp = StoreOp::Store;
            std::optional<Color> clearColor;
            std::optional<ClearDepthStencil> clearDepthStencil;
        };

        std::vector<AttachmentInfo> attachmentSpecifications;
    };

}