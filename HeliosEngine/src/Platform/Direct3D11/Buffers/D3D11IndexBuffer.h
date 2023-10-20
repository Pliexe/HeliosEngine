#pragma once

#include "pch.h"
#include "Helios/Resources/Buffer.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
    class D3D11IndexBuffer : public IndexBuffer
    {
    public:
        D3D11IndexBuffer(uint32_t* indices, uint32_t count, BufferUsage usage);
        D3D11IndexBuffer(uint32_t count, BufferUsage usage);
        ~D3D11IndexBuffer();

        uint32_t GetCount() const override;
        std::string ToString() const override;

        void SetData(const uint32_t* data, uint32_t count) override;

        void Bind() const override;
        void Unbind() const override;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
        uint32_t m_size;
        uint32_t m_count;
        uint32_t m_binding_slot;
        uint32_t* m_data;
    };
}
#endif