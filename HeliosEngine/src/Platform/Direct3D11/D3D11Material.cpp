#include "D3D11Material.h"

#ifdef HELIOS_PLATFORM_WINDOWS

#include "Direct3D11Context.h"
#include "Helios/Core/Logger.h"
#include "Helios/Core/Asserts.h"

namespace Helios {
    D3D11Material::D3D11Material(Filter filter, Type type) {
        D3D11_SAMPLER_DESC samplerDesc = {};
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));

        switch (filter) {
        case Filter::MinMagMipPoint:
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case Filter::ComparisonMinMagMipLinear:
			samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            break;
        default:
            HL_ASSERT(false, "Unknown Filter!");
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
            HL_ASSERT(false, "Unknown Type!");
            break;
        }

        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        samplerDesc.BorderColor[0] = 1.0f;
        samplerDesc.BorderColor[1] = 1.0f;
        samplerDesc.BorderColor[2] = 1.0f;
        samplerDesc.BorderColor[3] = 1.0f;

        HL_EXCEPTION(
            Direct3D11Context::GetCurrentContext()->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf()),
            "Failed to create sampler state!"
        );
    }
    
    void D3D11Material::Bind(uint32_t slot) {
        if(texture)
            texture->Bind();
        Direct3D11Context::GetCurrentContext()->GetContext()->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
        m_lastBoundSlot = slot;
    }

    void D3D11Material::Unbind() {
        if(texture)
            texture->Unbind();
        ID3D11SamplerState* nullss[] = {nullptr};
        Direct3D11Context::GetCurrentContext()->GetContext()->PSSetSamplers(m_lastBoundSlot, 1, nullss);
    }
}

#endif