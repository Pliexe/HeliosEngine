#include "Helios/Resources/Material.h"
#include "pch.h"

namespace Helios {
    class DirectXMaterial : public Material {
    public:
        DirectXMaterial(Filter filter, Type type);
        virtual ~DirectXMaterial() = default;

        virtual void Bind(uint32_t slot) override;
        virtual void Unbind() override;
    private:
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
        uint32_t m_lastBoundSlot;
    };
}