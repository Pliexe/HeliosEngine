#include "Helios/Resources/Buffer.h"

namespace Helios
{
    class VertexBuffer;
    class  DirectXVertexBuffer : public VertexBuffer
    {
    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
    public:
        DirectXVertexBuffer(uint32_t size, BufferUsage usage);
        DirectXVertexBuffer(const void* data, uint32_t size, BufferUsage usage);

        virtual void Bind(uint32_t slot = 0u) const override;
        virtual void Unbind() const override;

        virtual void SetData(const void* data, uint32_t size) override;
    };
}