#include "VkRenderPassCache.h"
#include "Helios/Core/Exceptions.h"

#include "VkImage.h"
#include "Helios/Graphics/GPURenderPass.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include <vulkan/vulkan_core.h>
#include "Platform/Vulkan/VkImage.h"

namespace Helios::Internal
{
    VkAttachmentLoadOp GetLoadOp(RenderPassBeginInfo::LoadOp load_op)
    {
        switch (load_op)
        {
            case RenderPassBeginInfo::LoadOp::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case RenderPassBeginInfo::LoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            case RenderPassBeginInfo::LoadOp::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
            default: return VK_ATTACHMENT_LOAD_OP_NONE;
        }
    }
    
    VkAttachmentStoreOp GetStoreOp(RenderPassBeginInfo::StoreOp store_op)
    {
        switch (store_op)
        {
            case RenderPassBeginInfo::StoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            case RenderPassBeginInfo::StoreOp::Store: return VK_ATTACHMENT_STORE_OP_STORE;
            default: return VK_ATTACHMENT_STORE_OP_NONE;
        }
    }

    VkRenderPassCacheKey::VkRenderPassCacheKey(const RenderPassBeginInfo& beginInfo)
    {
        HL_EXCEPTION(beginInfo.attachmentSpecifications.size() > MAX_ATTACHMENTS, std::format("Attachments may only be max size of: {}", MAX_ATTACHMENTS));
            std::memset(this, 0, sizeof(*this));

        attachmentCount = std::min(beginInfo.attachmentSpecifications.size(), MAX_ATTACHMENTS);
        subpassCount = 1;

        if (attachmentCount == 0) return;

        const auto& last = beginInfo.attachmentSpecifications[attachmentCount - 1];
        if (last.buffer->IsDepth())
            subpasses[0].depthStencilAttachment = static_cast<int32_t>(attachmentCount - 1);
        else
            subpasses[0].depthStencilAttachment = -1;

        subpasses[0].colorAttachmentCount = subpasses[0].depthStencilAttachment >= 0 
            ? static_cast<uint32_t>(attachmentCount - 1) 
            : static_cast<uint32_t>(attachmentCount);

        std::size_t i = 0;
        for (; i < subpasses[0].colorAttachmentCount; ++i)
        {
            auto& attachmentInfo = beginInfo.attachmentSpecifications[i];
            auto image = std::static_pointer_cast<VkImage>(attachmentInfo.buffer);
            auto& attachment = attachments[i];
            attachment = {
                .format = image->GetNativeFormat(),
                .loadOp = GetLoadOp(attachmentInfo.loadOp),
                .storeOp = GetStoreOp(attachmentInfo.storeOp),
            };

            if (image->IsSwapchain())
            {
                attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            else
            {
                if (attachmentInfo.loadOp == RenderPassBeginInfo::LoadOp::Load)
                {
                    // HL_EXCEPTION(true, "Not supported yet!");
                    attachment.initialLayout = image->GetDefaultLayout();
                }
                else
                {
                    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 
                }
                attachment.finalLayout = image->GetDefaultLayout();
            }
            subpasses[0].colorAttachments[i] = static_cast<uint32_t>(i);
        }
    }
    
    Vulkan::VkRenderPassWrapper& VkRenderPassCache::Acquire(const VkRenderPassCacheKey& key)
    {
        if (m_cache.Contains(key)) return m_cache.GetRef(key);

        std::vector<VkAttachmentDescription> attachments;
        attachments.reserve(key.attachmentCount);

        for (size_t i = 0; i < key.attachmentCount; i++)
        {
            const auto& att = key.attachments[i];
            VkAttachmentDescription attachmentDesc{};
            attachmentDesc.format = att.format;
            attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDesc.loadOp = att.loadOp;
            attachmentDesc.storeOp = att.storeOp;
            attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDesc.initialLayout = att.initialLayout;
            attachmentDesc.finalLayout = att.finalLayout;

            attachments.push_back(attachmentDesc);
        }

        std::vector<VkSubpassDescription> subpasses;
        std::vector<std::vector<VkAttachmentReference>> colorAttachmentRefs;

        subpasses.reserve(key.subpassCount);
        colorAttachmentRefs.resize(key.subpassCount);

        for (size_t i = 0; i < key.subpassCount; i++)
        {
            const auto& sp = key.subpasses[i];

            colorAttachmentRefs[i].resize(sp.colorAttachmentCount);
            for (uint32_t c = 0; c < sp.colorAttachmentCount; c++)
            {
                colorAttachmentRefs[i][c].attachment = sp.colorAttachments[c];
                colorAttachmentRefs[i][c].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }

            VkAttachmentReference* pDepthStencilAttachment = nullptr;
            VkAttachmentReference depthStencilRef{};
            if (sp.depthStencilAttachment >= 0)
            {
                depthStencilRef.attachment = static_cast<uint32_t>(sp.depthStencilAttachment);
                depthStencilRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                pDepthStencilAttachment = &depthStencilRef;
            }

            VkSubpassDescription subpassDesc{};
            subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDesc.colorAttachmentCount = sp.colorAttachmentCount;
            subpassDesc.pColorAttachments = colorAttachmentRefs[i].data();
            subpassDesc.pDepthStencilAttachment = pDepthStencilAttachment;

            subpasses.push_back(subpassDesc);
        }

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        auto result = Vulkan::VkRenderPassWrapper::Create(*m_device, &renderPassInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create Vulkan render pass!");

        assert(result.value() != VK_NULL_HANDLE);

        return m_cache.Insert(key, std::move(result.value()));
    }
}