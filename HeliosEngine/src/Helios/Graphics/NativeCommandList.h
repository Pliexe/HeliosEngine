#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Graphics/GraphicsPipeline.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/ShaderModule.h"
#include <initializer_list>

namespace Helios
{
    struct BindPipelineInfo
    {
        const std::vector<Ref<ShaderModule>>& vertexShaderModules;
        const std::vector<Ref<ShaderModule>>& pixelShaderModules;
        const std::vector<Ref<ShaderModule>>& geometryShaderModules = { };

        Topology topology = Topology::TriangleList;

        CullMode cullMode = CullMode::Back;
        FillMode fillMode = FillMode::Solid;
    };

    struct VertexBufferBindInfo
    {
        Ref<UnsafeVertexBuffer> buffer;
        uint32_t offset = 0;

        VertexBufferBindInfo(Ref<UnsafeVertexBuffer> buffer, uint32_t offset = 0) : buffer(buffer), offset(offset) {}
            
        template<typename T>
        VertexBufferBindInfo(const Ref<VertexBuffer<T>>& buffer, uint32_t offset = 0) : buffer(buffer->GetRaw()), offset(offset) {}
    };
    
    class HELIOS_API NativeCommandList
    {
    public:
    
        virtual void Reset() = 0;
        virtual void Compile() = 0;
        virtual void Execute() = 0;

        virtual void BeginRenderPass(const RenderPassBeginInfo& renderPassInfo) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindPipeline(const BindPipelineInfo& pipelineInfo) = 0;

        virtual void BindVertexBuffers(std::initializer_list<Ref<UnsafeVertexBuffer>> buffers) = 0;
        virtual void BindVertexBuffers(std::initializer_list<VertexBufferBindInfo> buffers) = 0;
        virtual void BindIndexBuffer(const Ref<IndexBuffer>& buffer, std::uint32_t offset = 0) = 0;

        virtual void BindUniformBuffer(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<UnsafeUniformBuffer>& buffer) = 0;
        virtual void BindTexture2D(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Texture2D>& buffer) = 0;
        virtual void BindImage(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Image>& buffer) = 0;
        virtual void BindSampler(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Sampler>& sampler) = 0;

        virtual void BindUniformBuffer(std::string_view name, const Ref<UnsafeUniformBuffer>& buffer) = 0;
        virtual void BindTexture2D(std::string_view name, const Ref<Texture2D>& buffer) = 0;
        virtual void BindImage(std::string_view name, const Ref<Image>& buffer) = 0;
        virtual void BindSampler(std::string_view name, const Ref<Sampler>& sampler) = 0;

        virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;
        virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex = 0, std::int32_t vertexOffset = 0, std::uint32_t firstInstance = 0) = 0;
        // virtual void DrawInstanced(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;

        // virtual void DrawIndexedInstanced(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;


    private:
    };

    class HELIOS_API NativeCommandListImmediate
    {
    public:
        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Reset() = 0;
        virtual void Execute() = 0;

        virtual void BeginRenderPass(const RenderPassBeginInfo& renderPassInfo) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindPipeline(const BindPipelineInfo& pipelineInfo) = 0;

        virtual void BindVertexBuffers(std::initializer_list<Ref<UnsafeVertexBuffer>> buffers) = 0;
        virtual void BindVertexBuffers(std::initializer_list<VertexBufferBindInfo> buffers) = 0;
        virtual void BindIndexBuffer(const Ref<IndexBuffer>& buffer, std::uint32_t offset) = 0;

        virtual void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) = 0;
        
    private:
    };
}