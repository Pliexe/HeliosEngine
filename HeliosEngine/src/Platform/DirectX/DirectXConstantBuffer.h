#pragma once

#include "Helios/Resources/Buffer.h"

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
                FAILED(hr = Graphics::instance->m_device->CreateBuffer(&bd, nullptr, &m_Data)),
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
                FAILED(hr = Graphics::instance->m_device->CreateBuffer(&bd, &sd, &m_Data)),
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
            Graphics::instance->m_deviceContext->PSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        }

        void BindVS(uint32_t slot)
        {
            HL_ASSERT_EXCEPTION(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, "Pipeline Stage: Vertex Shader.\nSlot: " + std::to_string(slot) + "\nSlot must be less than " + std::to_string(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) + "!");
            this->m_last_slot_vs = slot;
            Graphics::instance->m_deviceContext->VSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        }

        void BindGS(uint32_t slot)
        {
            HL_ASSERT_EXCEPTION(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, "Pipeline Stage: Vertex Shader.\nSlot: " + std::to_string(slot) + "\nSlot must be less than " + std::to_string(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) + "!");
            this->m_last_slot_gs = slot;
            Graphics::instance->m_deviceContext->GSSetConstantBuffers(slot, 1u, m_Data.GetAddressOf());
        }

        void UnbindPS() const
        {
            Graphics::instance->m_deviceContext->PSSetConstantBuffers(this->m_last_slot_ps, 1u, nullptr);
        }

        void UnbindVS() const
        {
            Graphics::instance->m_deviceContext->VSSetConstantBuffers(this->m_last_slot_vs, 1u, nullptr);
        }

        void UnbindGS() const
        {
            Graphics::instance->m_deviceContext->GSSetConstantBuffers(this->m_last_slot_gs, 1u, nullptr);
        }

        void SetData(T data)
            //void DirectXConstantBuffer::SetData(_DXBufferDataType data)
        {
            D3D11_MAPPED_SUBRESOURCE msr;
            HRESULT hr;
            HL_EXCEPTION_HR(FAILED(hr = Graphics::instance->m_deviceContext->Map(m_Data.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr)),
                "Failed to map constant buffer.", hr);
            std::memcpy(msr.pData, &data, sizeof(T));
            //memcpy(msr.pData, &data, sizeof(T));
            Graphics::instance->m_deviceContext->Unmap(m_Data.Get(), 0u);
        }

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Data;
		uint32_t m_Size;
        uint32_t m_last_slot_vs = 0u;
        uint32_t m_last_slot_ps = 0u;
        uint32_t m_last_slot_gs = 0u;
    };
}