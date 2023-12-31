#include <Platform/Direct3D11/DirectXFramebuffer.h>
#include "Framebuffer.h"

#include "Graphics.h"
#include "Helios/Core/Asserts.h"

namespace Helios
{
    Ref<Framebuffer> Helios::Framebuffer::Create(uint32_t width, uint32_t height)
    {
		return Create(width, height, { Format::R32G32B32A32F });
    }
    Ref<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height, std::initializer_list<Format> bufferSpecifications)
    {
        switch (Graphics::GetAPI())
        {
        case Graphics::API::Direct3D11: return CreateRef<DirectXFramebuffer>(width, height, bufferSpecifications);
        }

        HL_EXCEPTION(true, "No DepricatedGraphics API selected!");
        abort();
        return nullptr;
    }
}