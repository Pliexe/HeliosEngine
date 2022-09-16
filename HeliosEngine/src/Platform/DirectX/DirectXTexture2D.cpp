#include "DirectXTexture2D.h"
#include "stb_image.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Core/Asserts.h"

namespace Helios
{
    DirectXTexture2D::DirectXTexture2D(const std::string& path)
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
            FAILED(hr = Graphics::instance->m_device->CreateTexture2D(&textureDesc, &subResource, &m_Texture)),
            "Failed to create texture!\n" + GetLastErrorAsString(hr)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture1D.MipLevels = textureDesc.MipLevels;
        srvDesc.Texture1D.MostDetailedMip = 0;

        HL_EXCEPTION(
            FAILED(hr = Graphics::instance->m_device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, &m_TextureView)),
            "Failed to create shader resource view!\n" + GetLastErrorAsString(hr)
        );

        stbi_image_free(data);
    }

    DirectXTexture2D::DirectXTexture2D(const DirectXTexture2D& other)
    {
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_Texture = other.m_Texture;
        m_TextureView = other.m_TextureView;
    }

    void DirectXTexture2D::Bind(uint32_t slot) const
    {
        Graphics::instance->m_deviceContext->PSSetShaderResources(slot, 1, m_TextureView.GetAddressOf());
    }

    void DirectXTexture2D::Unbind() const
    {
        Graphics::instance->m_deviceContext->PSSetShaderResources(0, 1, nullptr);
    }
}