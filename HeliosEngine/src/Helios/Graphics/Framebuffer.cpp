#include <Platform/DirectX/DirectXFramebuffer.h>
#include "Framebuffer.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
    Ref<Framebuffer> Helios::Framebuffer::Create(uint32_t width, uint32_t height)
    {
        switch (Graphics::GetAPI())
        {
        case Graphics::API::DirectX: return CreateRef<DirectXFramebuffer>(width, height);
        }

        HL_EXCEPTION(true, "No Graphics API selected!");
        abort();
        return nullptr;
    }
}