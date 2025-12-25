#pragma once

#ifdef HELIOS_PLATFORM_WINDOWS

#include "Helios/Resources/Material.h"
#include "pch.h"

namespace Helios {
    class D3D11Material : public Material {
    public:
        D3D11Material(Filter filter, Type type);
        virtual ~D3D11Material() = default;

        virtual void Bind(uint32_t slot) override;
        virtual void Unbind() override;
    private:
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
        uint32_t m_lastBoundSlot = 0u;
    };
}

#endif