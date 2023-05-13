#pragma once

#include "Helios/Resources/Buffer.h"
#include "Direct3D11Context.h"

namespace Helios
{
    template <typename T>
    class DirectXConstantBuffer : public ConstantBuffer<T>
    {
    public:
        DirectXConstantBuffer(uint32_t size) : m_Size(size)
        {
            D3D11_BUFFER_DESC bd = { };
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0u;
            bd.ByteWidth = size;
            bd.StructureByteStride = 0u;

            HRESULT hr;
            HL_EXCEPTION(
                FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, nullptr, &m_Data)),
                std::string("Failed to create Constant Buffer!") + "\nError: " + GetLastErrorAsString(hr)
            );
        }

        DirectXConstantBuffer(T data, uint32_t size) : m_Size(size)
        {
            D3D11_BUFFER_DESC bd = { };
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bd.MiscFlags = 0u;
            bd.ByteWidth = size;
            bd.StructureByteStride = 0u;
            D3D11_SUBRESOURCE_DATA sd = {};
            sd.pSysMem = &data;
            HRESULT hr;
            HL_EXCEPTION(
                FAILED(hr = Direct3D11Context::GetCurrentContext()->GetDevice()->CreateBuffer(&bd, &sd, &m_Data)),
                std::string("Failed to create Constant Buffer!") + "\nError: " + GetLastErrorAsString(hr)
            );
        }

        ~DirectXConstantBuffer()
        {
            // WRL smart pointers will automatically release the COM object
        }

        void BindPS(uint32_t slot)
        {
            HL_CORE_ASSERT_WITH_MSG(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, "Pipeline Stage: Pixel Shader.\nSlot: " + std::to_string(slot) + "\nSlot must be less than " + std::to_string(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) + "!");
            this->m_last_slot_ps = slot;
            Direct3D11Context::GetCurrentContext()->GetContext()->PSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        }

        void BindVS(uint32_t slot)
        {
            HL_ASSERT_EXCEPTION(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, "Pipeline Stage: Vertex Shader.\nSlot: " + std::to_string(slot) + "\nSlot must be less than " + std::to_string(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) + "!");
            this->m_last_slot_vs = slot;
            Direct3D11Context::GetCurrentContext()->GetContext()->VSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        }

        void BindGS(uint32_t slot)
        {
            HL_ASSERT_EXCEPTION(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, "Pipeline Stage: Vertex Shader.\nSlot: " + std::to_string(slot) + "\nSlot must be less than " + std::to_string(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) + "!");
            this->m_last_slot_gs = slot;
            Direct3D11Context::GetCurrentContext()->GetContext()->GSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        }

        void UnbindPS() const
        {
            Direct3D11Context::GetCurrentContext()->GetContext()->PSSetConstantBuffers(this->m_last_slot_ps, 1u, nullptr);
        }

        void UnbindVS() const
        {
            Direct3D11Context::GetCurrentContext()->GetContext()->VSSetConstantBuffers(this->m_last_slot_vs, 1u, nullptr);
        }

        void UnbindGS() const
        {
            Direct3D11Context::GetCurrentContext()->GetContext()->GSSetConstantBuffers(this->m_last_slot_gs, 1u, nullptr);
        }

        void SetData(T data)
            //void DirectXConstantBuffer::SetData(_DXBufferDataType data)
        {
            D3D11_MAPPED_SUBRESOURCE msr;
            HRESULT hr;
            HL_EXCEPTION_HR(FAILED(hr = Direct3D11Context::GetCurrentContext()->GetContext()->Map(m_Data.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr)),
                "Failed to map constant buffer.", hr);
            std::memcpy(msr.pData, &data, sizeof(T));
            //memcpy(msr.pData, &data, sizeof(T));
            Direct3D11Context::GetCurrentContext()->GetContext()->Unmap(m_Data.Get(), 0u);
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Data;
		uint32_t m_Size;
        uint32_t m_last_slot_vs = 0u;
        uint32_t m_last_slot_ps = 0u;
        uint32_t m_last_slot_gs = 0u;
    };
}