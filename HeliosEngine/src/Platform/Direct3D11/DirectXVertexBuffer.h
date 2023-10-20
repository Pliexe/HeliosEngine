#include "Helios/Resources/Buffer.h"

namespace Helios
{
    class DepricatedVertexBuffer;
    class  DepricatedDirectXVertexBuffer : public DepricatedVertexBuffer
    {
    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
    public:
        DepricatedDirectXVertexBuffer(uint32_t size, BufferUsage usage);
        DepricatedDirectXVertexBuffer(const void* data, uint32_t size, BufferUsage usage);

        virtual void Bind(uint32_t slot = 0u) const override;
        virtual void Unbind() const override;

        virtual void SetData(const void* data, uint32_t size) override;
    };
}