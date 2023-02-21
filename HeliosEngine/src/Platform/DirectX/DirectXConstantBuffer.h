#pragma once

#include "Helios/Resources/Buffer.h"

namespace Helios
{
    class DirectXConstantBuffer : public virtual ConstantBuffer
    {
    public:
        DirectXConstantBuffer(uint32_t size);
        DirectXConstantBuffer(const void* data, uint32_t size);
        ~DirectXConstantBuffer();

        void BindPS(uint32_t slot = 0) override;
        void BindVS(uint32_t slot = 0) override;
        void UnbindPS() const override;
        void UnbindVS() const override;

        void SetData(const void* data, uint32_t size) override;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Data;
        uint32_t last_slot_vs = 0u;
        uint32_t last_slot_ps = 0u;
    };
}