#pragma once

#include "pch.h"
#include "Helios/Resources/Buffer.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
    class D3D11VertexBuffer : public UnsafeVertexBuffer
    {
    public:
        D3D11VertexBuffer(const void* data, uint32_t size, BufferUsage usage);
        D3D11VertexBuffer(uint32_t size, BufferUsage usage);

        void Bind(uint32_t slot = 0u) const override;
        void Unbind(uint32_t slot = 0u) const override;
        void SetData(const void* data, uint32_t size) override;
        void SetInputLayout(const InputLayout& layout) override;
        const InputLayout& GetInputLayout() const override;

        friend class D3D11VertexArray;
    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
        uint32_t m_Stride;
        InputLayout m_InputLayout;
        BufferUsage m_Usage;
    };
}
#endif