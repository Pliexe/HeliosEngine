#pragma once

#include "Helios/Resources/Buffer.h"

namespace Helios
{
    class VkVertexBuffer : public UnsafeVertexBuffer
    {
    public:
    public:
        VkVertexBuffer(const void* data, uint32_t size, BufferUsage usage);
        VkVertexBuffer(uint32_t size, BufferUsage usage);

        void Bind(uint32_t slot = 0u) const override;
        void Unbind(uint32_t slot = 0u) const override;
        void SetData(const void* data, uint32_t size) override;
        void SetInputLayout(const InputLayout& layout) override;
        const InputLayout& GetInputLayout() const override;
    };
}