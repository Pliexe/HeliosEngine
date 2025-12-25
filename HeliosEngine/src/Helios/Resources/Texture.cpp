#include "Texture.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"
#include <webp/decode.h>
#include <stb_image.h>

#ifdef HELIOS_PLATFORM_WINDOWS
#include "Platform/Direct3D11/Direct3D11Texture2D.h"
#endif

namespace Helios
{
    Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
    {
        return Graphics::GetMainDevice()->CreateTexture2D(path);
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        return Graphics::GetMainDevice()->CreateTexture2D(width, height);
    }

    bool IsWebPFile(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
            return false;

        std::array<char, 12> header{};
        file.read(header.data(), header.size());
        if (file.gcount() < 12)
            return false;

        // file header
        return (header[0] == 'R' && header[1] == 'I' && header[2] == 'F' && header[3] == 'F' &&
                header[8] == 'W' && header[9] == 'E' && header[10] == 'B' && header[11] == 'P');
    }

    Texture2D::Texture2D(const Ref<GraphicsDevice>& device, const std::filesystem::path& path)
    {
        int texWidth, texHeight, texChannels;
        std::string pathStr = conversions::from_u8string(path.u8string());
        stbi_uc* pixels = nullptr;

        auto isWebp = IsWebPFile(path);

        if (isWebp)
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            HL_EXCEPTION(!file, "Failed to open WebP image at path: " + pathStr);

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> fileData(size);
            HL_EXCEPTION(!file.read(reinterpret_cast<char*>(fileData.data()), size), "Failed to read WebP image at path: " + pathStr);

            uint8_t* data = WebPDecodeRGBA(fileData.data(), size, &texWidth, &texHeight);
            HL_EXCEPTION(!data, "Failed to decode WebP image at path: " + pathStr);

            pixels = data;
        }
        else
        {
            pixels = stbi_load(pathStr.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            HL_EXCEPTION(!pixels, "Failed to load image at path: {} \nReason: {}", pathStr, stbi_failure_reason());
        }

        for (int y = 0; y < texHeight / 2; ++y)
            std::swap(pixels[y * texWidth * 4], pixels[(texHeight - 1 - y) * texWidth * 4]);

        try {
            m_image = device->CreateImage(
                Image::Usage::Sampled | Image::Usage::TransferDst,
                Image::Format::R8G8B8A8_UNORM,
                texWidth,
                texHeight
            );

            m_image->SetData(pixels, texWidth * texHeight * 4);

            
        } catch (const std::exception& e) {
            if (isWebp)
                WebPFree(pixels);
            else
                stbi_image_free(pixels);

            HL_EXCEPTION(true, "Failed to create texture at path: " + pathStr + " : " + std::string(e.what()));
        }
        if (isWebp)
            WebPFree(pixels);
        else
            stbi_image_free(pixels);
    }

    Texture2D::Texture2D(const Ref<GraphicsDevice>& device, uint32_t width, uint32_t height)
    {
        m_image = device->CreateImage(
            Image::Usage::Sampled | Image::Usage::TransferDst,
            Image::Format::R8G8B8A8_UNORM,
            width,
            height
        );
    }
}
