#include "Texture.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Core/Asserts.h"
#include "Platform/DirectX/DirectXTexture2D.h"

namespace Helios
{
    Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
    {
        switch (Graphics::GetAPI())
        {
        case Graphics::API::DirectX: return CreateRef<DirectXTexture2D>(path.string());
        }

        HL_CORE_ASSERT_WITH_MSG(false, "Unknown Graphics API!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Graphics::GetAPI())
        {
        case Graphics::API::DirectX: return CreateRef<DirectXTexture2D>(width, height);
        }

        HL_CORE_ASSERT_WITH_MSG(false, "Unknown Graphics API!");
        return nullptr;
    }
}