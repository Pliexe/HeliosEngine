#include "Direct3D11Texture2D.h"

#include "Direct3D11Context.h"
#include "Helios/Core/Asserts.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Helios
{
    Direct3D11Texture2D::Direct3D11Texture2D(const std::string& path)
    {
        int width, height;
        //stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        {
            data = stbi_load(path.c_str(), &width, &height, nullptr, 4);
        }
        HL_EXCEPTION(
			!data,
			"Failed to load image at path: " + path
        )
        //HL_CORE_ASSERT_WITH_MSG(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = (UINT)m_Width;
        textureDesc.Height = (UINT)m_Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = data;
        subResource.SysMemPitch = textureDesc.Width * 4;
        subResource.SysMemSlicePitch = 0;

        HRESULT hr;

        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateTexture2D(&textureDesc, &subResource, &m_Texture)),
            "Failed to create texture!\n" + GetLastErrorAsString(hr)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture1D.MipLevels = textureDesc.MipLevels;
        srvDesc.Texture1D.MostDetailedMip = 0;

        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateShaderResourceView(m_Texture.Get(), &srvDesc, &m_TextureView)),
            "Failed to create shader resource view!\n" + GetLastErrorAsString(hr)
        );

        stbi_image_free(data);
    }

    Direct3D11Texture2D::Direct3D11Texture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(textureDesc));
        textureDesc.Width = (UINT)m_Width;
        textureDesc.Height = (UINT)m_Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        textureDesc.MiscFlags = 0;

        HRESULT hr;

        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_Texture)),
            "Failed to create texture!\n" + GetLastErrorAsString(hr)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));

        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        HL_EXCEPTION(
            FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateShaderResourceView(m_Texture.Get(), &srvDesc, &m_TextureView)),
            "Failed to create shader resource view!\n" + GetLastErrorAsString(hr)
        );
    }

    Direct3D11Texture2D::Direct3D11Texture2D(const Direct3D11Texture2D& other)
    {
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_Texture = other.m_Texture;
        m_TextureView = other.m_TextureView;
    }

    void Direct3D11Texture2D::SetData(void* data, uint32_t size)
    {
        Direct3D11Context::GetCurrentContext()->GetContext()->UpdateSubresource(m_Texture.Get(), 0, nullptr, data, m_Width * 4, 0);
    }

    void Direct3D11Texture2D::Bind(uint32_t slot)
    {
        Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShaderResources(slot, 1, m_TextureView.GetAddressOf());
        m_lastBoundSlot = slot;
    }

    void Direct3D11Texture2D::Unbind()
    {
        Direct3D11Context::GetCurrentContext()->GetContext()->PSSetShaderResources(m_lastBoundSlot, 1, nullptr);
    }
}