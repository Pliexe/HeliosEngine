#include "Texture.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"
#include "Platform/Direct3D11/Direct3D11Texture2D.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"

namespace Helios
{
    Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
    {
        switch (Graphics::GetAPI())
        {
        case Graphics::API::Direct3D11: return CreateRef<Direct3D11Texture2D>(path.string());
        case Graphics::API::OpenGL: return CreateRef<OpenGLTexture2D>(path.string());
        }

        HL_ASSERT(false, "Unknown DepricatedGraphics API!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Graphics::GetAPI())
        {
        case Graphics::API::Direct3D11: return CreateRef<Direct3D11Texture2D>(width, height);
        case Graphics::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
        }

        HL_ASSERT(false, "Unknown DepricatedGraphics API!");
        return nullptr;
    }
}
