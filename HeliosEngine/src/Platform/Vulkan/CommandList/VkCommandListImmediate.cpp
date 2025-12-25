
#include "VkCommandListImmediate.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Logger.h"
#include "Helios/Graphics/GPURenderPass.h"
#include "Platform/Vulkan/Buffers/VkIndexBuffer.h"
#include "Platform/Vulkan/Buffers/VkVertexBuffer.h"
#include "Platform/Vulkan/CommandList/VkCommandListHelpers.h"
#include "Platform/Vulkan/VkFramebufferCache.h"
#include "Platform/Vulkan/VkImage.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{

    static bool IsRecorded(bool isRecorded)
    {
        if (isRecorded) 
        {
            HL_MESSAGE("The CommandList is already compiled! You can't submit commands after compiling it! Please reset it before submiting");
            return true;
        }
        return false;
    }

    VkCommandListImmediate::VkCommandListImmediate(const Ref<VkGraphicsDevice>& device, VkCommandPool commandPool, bool isTransient)
    {
        m_isTransient = isTransient;
        m_device = device;
        m_commandPool = commandPool;
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        HL_EXCEPTION(vkAllocateCommandBuffers(*device, &allocInfo, &m_commandBuffer) != VK_SUCCESS,
        "Failed to allocate command buffers!");
    }

    VkCommandListImmediate::~VkCommandListImmediate()
    {
        m_device->GetSyncPool().Wait(m_lastTicket);
        vkFreeCommandBuffers(*m_device, m_commandPool, 1, &m_commandBuffer);
    }

    void VkCommandListImmediate::Reset()
    {
        m_device->GetSyncPool().Wait(m_lastTicket);
        m_resourceSync.Clear();
        vkResetCommandBuffer(m_commandBuffer, 0);
        m_vbValidMask = 0;
        m_boundVbLayouts.fill(std::nullopt);
        m_recorded = false;
    }

    void VkCommandListImmediate::Execute()
    {
        IsRecorded(!m_recorded);

        VkGraphicsDevice::SubmitInfo submitInfo{};
        submitInfo.commandBuffers = &m_commandBuffer;
        submitInfo.commandBufferCount = 1;
        submitInfo.signalSemaphore = VK_NULL_HANDLE;

        VkGraphicsDevice::SubmitResourceInfo resourceInfo{};
        resourceInfo.readResources = m_resourceSync.readResources;
        resourceInfo.writeResources = m_resourceSync.writeResources;

        m_lastTicket = m_device->Submit(submitInfo, resourceInfo);
    }

    void VkCommandListImmediate::Begin()
    {
        if(IsRecorded(m_recorded)) return;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        HL_EXCEPTION(vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS, "Failed to begin recording command buffer!");
    }

    void VkCommandListImmediate::End()
    {
        if(IsRecorded(m_recorded)) return;

        HL_EXCEPTION(vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS, "Failed to end recording command buffer!");

        // Normalize resources
        m_resourceSync.Normalize();

        m_recorded = true;        
    }

    void VkCommandListImmediate::BeginRenderPass(const RenderPassBeginInfo& renderPassInfo)
    {
        if(IsRecorded(m_recorded)) return;
        
        if (m_in_renderPass) 
        {
            HL_MESSAGE("Already is already started!");
            return;
        }


        std::vector<VkClearValue> clearValues;
        clearValues.reserve(renderPassInfo.attachmentSpecifications.size());
        for (auto& attachmentSpec : renderPassInfo.attachmentSpecifications)
        {
            if (attachmentSpec.buffer->IsDepth())
            {
                if (attachmentSpec.clearDepthStencil.has_value())
                {
                    auto& clearDepthStencil = attachmentSpec.clearDepthStencil.value();
                    clearValues.emplace_back(VkClearValue { .depthStencil { .depth = clearDepthStencil.depth, .stencil = clearDepthStencil.stencil } });
                } else clearValues.emplace_back(VkClearValue { .color = {{0.f, 0.f, 0.f, 1.f}} });
            } else 
            {
                if (attachmentSpec.clearColor.has_value())
                {
                    auto& color = attachmentSpec.clearColor.value();
                    clearValues.emplace_back(VkClearValue { .color = { { color.r, color.g, color.b, color.a } } });
                } else {
                    clearValues.emplace_back(VkClearValue { .color = {{0.f, 0.f, 0.f, 1.f}} });
                }
            }
        }

        auto passKey = VkRenderPassCacheKey(renderPassInfo);
                    
        auto& renderPass = m_device->GetRenderPassCache().Acquire(passKey);
        auto size = renderPassInfo.attachmentSpecifications[0].buffer->GetSize();
        assert(renderPass.Get() != VK_NULL_HANDLE);
        
        std::vector<::VkImageView> images;
        images.reserve(renderPassInfo.attachmentSpecifications.size());
        VkFramebufferCacheKey vkfbck{};
        
        for (auto& attach : renderPassInfo.attachmentSpecifications)
        {
            images.push_back(std::static_pointer_cast<VkImage>(attach.buffer)->GetViewImage());
        }
        vkfbck.attachmentCount = images.size();
        std::copy(images.begin(), images.end(), vkfbck.attachments);
        vkfbck.renderPassCacheKey = passKey;
        vkfbck.width = size.width;
        vkfbck.height = size.height;
        auto& framebuffer = m_device->GetFramebufferCache().Acquire(vkfbck);

        VkRenderPassBeginInfo rpBegin{};
        rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpBegin.renderPass = renderPass.Get();
        rpBegin.framebuffer = framebuffer.Get();
        rpBegin.renderArea.offset = { 0, 0 };
        rpBegin.renderArea.extent =  { .width = size.width, .height = size.height, };

        rpBegin.clearValueCount = static_cast<uint32_t>(clearValues.size());
        rpBegin.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffer, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(size.width);
        viewport.height = static_cast<float>(size.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = {size.width, size.height};
        vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

        for (auto& attach : renderPassInfo.attachmentSpecifications)
        {
            m_resourceSync.AddWrite(std::static_pointer_cast<VkImage>(attach.buffer));
        }
        
        m_lastRenderpass = CreateRef<RenderPassBeginInfo>(renderPassInfo);
        m_in_renderPass = true;
    }
    void VkCommandListImmediate::EndRenderPass()
    {    
        if(IsRecorded(m_recorded)) return;
        
        if (!m_in_renderPass) 
        {
            HL_MESSAGE("RenderPass was not started!");
            return;
        }

        vkCmdEndRenderPass(m_commandBuffer);

        m_lastRenderpass.reset();
        m_lastBoundPipelineInputLayouts.reset();
        m_lastBoundPipeline = VK_NULL_HANDLE;
        m_in_renderPass = false;
    }

    void VkCommandListImmediate::BindPipeline(const BindPipelineInfo& pipelineInfo)
    {
        if(IsRecorded(m_recorded)) return;

        BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
        
        BIND_ERROR(pipelineInfo.vertexShaderModules.empty(), "Pipeline must contain at least 1 Vertex Module!");
        BIND_ERROR(pipelineInfo.pixelShaderModules.empty(), "Pipeline must contain at least 1 Pixel Module!");

        auto HasMain = [](const std::vector<Ref<ShaderModule>>& mods) { 
            return std::find_if(mods.begin(), mods.end(), [](Ref<ShaderModule> x){ return x->GetFamily() == ShaderModule::Family::Main; }); 
        };

        auto vsMain = HasMain(pipelineInfo.vertexShaderModules);
        BIND_ERROR(vsMain == pipelineInfo.vertexShaderModules.end(), "Pipeline must contain at least 1 Vertex Module that is of Family::Main!");
        BIND_ERROR(HasMain(pipelineInfo.pixelShaderModules) == pipelineInfo.vertexShaderModules.end(), "Pipeline must contain at least 1 Pixel Module that is of Family::Main!");
        if (!pipelineInfo.geometryShaderModules.empty()) BIND_ERROR(HasMain(pipelineInfo.geometryShaderModules) == pipelineInfo.vertexShaderModules.end(), "Pipeline must contain at least 1 Geometry Module that is of Family::Main!");
        

        auto pipeline = m_device->GetGraphicsPipelineCache().Acquire(*m_lastRenderpass, 0, pipelineInfo);

        if (pipeline == nullptr) return;
        
        if (m_lastBoundPipeline == pipeline->m_pipeline) return;

        vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->m_pipeline);
        m_lastBoundPipeline = pipeline->m_pipeline;

        if (m_lastBoundPipelineInputLayouts.has_value())
        {
            auto vsInputLayouts = vsMain->get()->GetInputLayouts();
            if (vsInputLayouts.has_value())
            {
                const auto& newLayouts = vsInputLayouts->GetLayouts();
                for (uint32_t i = 0; i < newLayouts.size(); ++i)
                {
                    if (!(m_vbValidMask & (1 << i))) continue;
                    if (m_boundVbLayouts[i].has_value() && !(newLayouts[i] == *m_boundVbLayouts[i]))
                    {
                        m_vbValidMask &= ~(1 << i);
                    }
                }
            }
        }

        m_lastBoundPipelineInputLayouts = vsMain->get()->GetInputLayouts();
    }

    void VkCommandListImmediate::BindVertexBuffers(std::initializer_list<Ref<UnsafeVertexBuffer>> buffers)
    {
        std::vector<VertexBufferBindInfo> bindInfos;
        bindInfos.reserve(buffers.size());
        for (auto& buffer : buffers) bindInfos.emplace_back(buffer, 0);

        if (IsRecorded(m_recorded)) return;

        BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
        BIND_ERROR(m_lastBoundPipeline == VK_NULL_HANDLE, "No bound pipeline! Can't bind resource outside pipeline.");

        BIND_ERROR(bindInfos.size() != m_lastBoundPipelineInputLayouts->size(), 
        std::format("VertexBuffers must match the input layout used by the VertexShader! Missatching sizes: {} != {}", bindInfos.size(), m_lastBoundPipelineInputLayouts->size()).c_str()
        );

        const auto& pipelineLayouts = m_lastBoundPipelineInputLayouts->GetLayouts();

        std::vector<VkBuffer> vkBuffers;
        std::vector<VkDeviceSize> offsets;
        vkBuffers.resize(bindInfos.size());
        offsets.resize(bindInfos.size());

        for (std::size_t i=0, n=bindInfos.size(); i < n; ++i)
        {
            auto& bindInfo = bindInfos[i];
            auto buff = std::static_pointer_cast<VkVertexBuffer>(bindInfo.buffer);
            uint32_t stride = pipelineLayouts[i].GetStride();

            BIND_ERROR(buff->GetSize() < bindInfo.offset + stride,
            std::format("VertexBuffer at index {} is too small for the expected stride! Size: {}, Offset: {}, Stride: {}", 
            i, buff->GetSize(), bindInfo.offset, stride).c_str());

            vkBuffers[i] = buff->GetBuffer();
            offsets[i] = bindInfo.offset;

            m_vbValidMask |= (1 << i);
            m_boundVbLayouts[i] = pipelineLayouts[i];

            m_resourceSync.AddRead(buff);
        }
        
        vkCmdBindVertexBuffers(m_commandBuffer, 0, bindInfos.size(), vkBuffers.data(), offsets.data());
    }

    void VkCommandListImmediate::BindVertexBuffers(std::initializer_list<VertexBufferBindInfo> buffers)
    {
        if (IsRecorded(m_recorded)) return;

        BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
        BIND_ERROR(m_lastBoundPipeline == VK_NULL_HANDLE, "No bound pipeline! Can't bind resource outside pipeline.");

        BIND_ERROR(buffers.size() != m_lastBoundPipelineInputLayouts->size(), 
        std::format("VertexBuffers must match the input layout used by the VertexShader! Missatching sizes: {} != {}", buffers.size(), m_lastBoundPipelineInputLayouts->size()).c_str()
        );

        const auto& pipelineLayouts = m_lastBoundPipelineInputLayouts->GetLayouts();

        std::vector<VkBuffer> vkBuffers;
        std::vector<VkDeviceSize> offsets;
        vkBuffers.resize(buffers.size());
        offsets.resize(buffers.size());

        for (std::size_t i=0, n=buffers.size(); i < n; ++i)
        {
            const auto& bindInfo = *(buffers.begin() + i);
            auto buff = std::static_pointer_cast<VkVertexBuffer>(bindInfo.buffer);
            uint32_t stride = pipelineLayouts[i].GetStride();

            BIND_ERROR(buff->GetSize() < bindInfo.offset + stride,
            std::format("VertexBuffer at index {} is too small for the expected stride! Size: {}, Offset: {}, Stride: {}", 
            i, buff->GetSize(), bindInfo.offset, stride).c_str());

            vkBuffers[i] = buff->GetBuffer();
            offsets[i] = bindInfo.offset;

            m_vbValidMask |= (1 << i);
            m_boundVbLayouts[i] = pipelineLayouts[i];

            m_resourceSync.AddRead(buff);
        }
        
        vkCmdBindVertexBuffers(m_commandBuffer, 0, buffers.size(), vkBuffers.data(), offsets.data());
    }

    void VkCommandListImmediate::BindIndexBuffer(const Ref<IndexBuffer>& buffer, std::uint32_t offset)
    {
        if (IsRecorded(m_recorded)) return;

        BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
        
        vkCmdBindIndexBuffer(m_commandBuffer, std::static_pointer_cast<VkIndexBuffer>(buffer)->GetBuffer(), offset, VK_INDEX_TYPE_UINT32);
        
        m_resourceSync.AddRead(std::static_pointer_cast<VkIndexBuffer>(buffer));
    }

    void VkCommandListImmediate::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
    {
        if (IsRecorded(m_recorded)) return;

        BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
        BIND_ERROR(m_lastBoundPipeline == VK_NULL_HANDLE, "No bound pipeline!");

        uint32_t requiredMask = (1 << m_lastBoundPipelineInputLayouts->size()) - 1;
        BIND_ERROR((m_vbValidMask & requiredMask) != requiredMask,
            "One or more vertex buffer slots are empty or incompatible with the current pipeline layout!");

        vkCmdDraw(
            m_commandBuffer, 
            vertexCount, 
            1, 
            firstVertex, 
            0
        );
    }
}