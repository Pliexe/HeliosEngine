#include "DirectXMaterial.h"
#include "Helios/Core/Logger.h"
#include "Helios/Core/Asserts.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios {
    DirectXMaterial::DirectXMaterial(Filter filter, Type type) {
        D3D11_SAMPLER_DESC samplerDesc = {};
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));

        switch (filter) {
        case Filter::MinMagPoint:
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        default:
            HL_CORE_ASSERT(false, "Unknown Filter!");
            break;
        }

        switch (type) {
        case Type::Warp:
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            break;
        case Type::Mirror:
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
            break;
        case Type::Clamp:
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case Type::Border:
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
            break;
        case Type::MirrorOnce:
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
            break;
        default:
            HL_CORE_ASSERT(false, "Unknown Type!");
            break;
        }

        HL_EXCEPTION(
            Graphics::instance->m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf()),
            "Failed to create sampler state!"
        );
    }
    
    void DirectXMaterial::Bind(uint32_t slot) {
        if(texture)
            texture->Bind();
        Graphics::instance->m_deviceContext->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
        m_lastBoundSlot = slot;
    }

    void DirectXMaterial::Unbind() {
        if(texture)
            texture->Unbind();
        Graphics::instance->m_deviceContext->PSSetSamplers(m_lastBoundSlot, 1, nullptr);
    }
}