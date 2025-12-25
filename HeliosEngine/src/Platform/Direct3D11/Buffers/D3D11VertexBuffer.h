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

        void SetData(const void* data, uint32_t size) override;
        uint32_t GetSize() const override { return m_Size; }

        friend class D3D11VertexArray;
    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
        uint32_t m_Size;
        BufferUsage m_Usage;
    };
}
#endif