#include "EngineTexture.h"
#include "Helios/Graphics/Graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Helios {
	
	EngineTexture::EngineTexture(const char* filename) {
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL) throw std::runtime_error(std::string("Unable to load image: ") + filename);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = image_width;
		desc.Height = image_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = image_data;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		Graphics::instance->m_device->CreateTexture2D(&desc, &subResource, &pTexture);
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture1D.MipLevels = desc.MipLevels;
		srvDesc.Texture1D.MostDetailedMip = 0;
		Graphics::instance->m_device->CreateShaderResourceView(pTexture, &srvDesc, &this->shaderResourceView);
		pTexture->Release();

		stbi_image_free(image_data);
	}

	EngineTexture::operator void*()
	{
		return (void*)shaderResourceView.Get();
	}
}