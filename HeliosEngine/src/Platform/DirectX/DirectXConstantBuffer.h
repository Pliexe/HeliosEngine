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

        void Bind(uint32_t slot = 0) const override;
        void Unbind() const override;

        void SetData(const void* data, uint32_t size) override;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Data;
    };
}