#pragma once

#include "Helios/Graphics/GPURenderPass.h"
#include "Helios/Graphics/NativeCommandList.h"
#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    class VkCommandListImmediate final : public NativeCommandListImmediate
    {
    public:
        VkCommandListImmediate(const Ref<VkGraphicsDevice>& device, VkCommandPool commandPool, bool isTransient);
        ~VkCommandListImmediate();
        
        VkCommandBuffer GetCommandBuffer() const { return m_commandBuffer; }

        void Begin() override;
        void End() override;
        
        void Reset() override;
        void Execute() override;

        void BeginRenderPass(const RenderPassBeginInfo& renderPassInfo) override;
        void EndRenderPass() override;

        void BindPipeline(const BindPipelineInfo& pipelineInfo) override;

        void BindVertexBuffers(std::initializer_list<Ref<UnsafeVertexBuffer>> buffers) override;
        void BindVertexBuffers(std::initializer_list<VertexBufferBindInfo> buffers) override;
        void BindIndexBuffer(const Ref<IndexBuffer>& buffer, std::uint32_t offset) override;

        void Draw(std::uint32_t vertexCount, std::uint32_t firstVertex) override;

        void AppendCommand();
    private:
        friend class VkContext;
        friend class VkCommandList;
    
        VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
        VkResourceSyncList m_resourceSync;
        SyncTicket m_lastTicket;
        bool m_recorded = false;
        bool m_isTransient = false;
        Ref<VkGraphicsDevice> m_device;

        Ref<RenderPassBeginInfo> m_lastRenderpass;
        VkPipeline m_lastBoundPipeline = VK_NULL_HANDLE;
        std::optional<InputLayouts> m_lastBoundPipelineInputLayouts;
        uint32_t m_vbValidMask = 0;
        std::array<std::optional<InputLayout>, 32> m_boundVbLayouts;
        bool m_in_renderPass = false;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
    };
}