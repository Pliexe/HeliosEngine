#include "VkCommandList.h"


#include "Helios/Core/Base.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Logger.h"
#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/Buffers/VkIndexBuffer.h"
#include "Platform/Vulkan/Buffers/VkVertexBuffer.h"
#include "Platform/Vulkan/CommandList/VkCommandListHelpers.h"
#include "Platform/Vulkan/GraphicsPipeline/VkGraphicsPipelineCache.h"
#include "Platform/Vulkan/VkFramebufferCache.h"
#include "Platform/Vulkan/VkImage.h"
#include "Platform/Vulkan/VkRenderPassCache.h"
#include <memory>
#include <optional>
#include <type_traits>
#include <type_traits>
#include <vulkan/vulkan_core.h>


namespace Helios::Internal
{

static bool IsRecorded(bool isRecorded)
{
    if (isRecorded)
    {
        HL_MESSAGE(
            "The CommandList is already compiled! You can't submit commands "
            "after compiling it! Please reset it before submiting");
        return true;
    }
    return false;
}

VkCommandList::VkCommandList(const Ref<VkGraphicsDevice>& device,
                             VkCommandPool commandPool, bool isTransient)
{
    m_device = device;
    m_commandPool = commandPool;
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    HL_EXCEPTION(vkAllocateCommandBuffers(*device, &allocInfo, &m_commandBuffer) != VK_SUCCESS, "Failed to allocate command buffers!");
}

VkCommandList::~VkCommandList()
{
    // vkWaitForFences(*m_device, 1, &m_sync.fence, VK_TRUE, UINT64_MAX);
    if (m_lastTicket.id != 0) m_device->GetSyncPool().Wait(m_lastTicket);

    vkFreeCommandBuffers(*m_device, m_commandPool, 1, &m_commandBuffer);
}

void VkCommandList::Reset()
{
    if (!m_recorded)
        return;

    // vkWaitForFences(*m_device, 1, &m_sync.fence, VK_TRUE, UINT64_MAX);
    if (m_lastTicket.id != 0) m_device->GetSyncPool().Wait(m_lastTicket);

    m_commands.clear();
    m_resourceSync.Clear();
    m_waitSemaphores.clear(); // Keep for now if used elsewhere, or remove if unused. User said it's old way.
    vkResetCommandBuffer(m_commandBuffer, 0);
    m_vbValidMask = 0;
    m_boundVbLayouts.fill(std::nullopt);
    m_recorded = false;
}

void VkCommandList::Execute()
{
    if (!m_recorded)
        Compile();

    // TODO: Layout Transitions - I'll do it when I'm ready to handle it. (After I'm done with the sync pool)
   

    VkGraphicsDevice::SubmitInfo submitInfo{};
    submitInfo.commandBuffers = &m_commandBuffer;
    submitInfo.commandBufferCount = 1;
    
    submitInfo.signalSemaphore = VK_NULL_HANDLE;
    submitInfo.dependency = m_lastTicket;

    VkGraphicsDevice::SubmitResourceInfo resourceInfo{};
    resourceInfo.readResources = m_resourceSync.readResources;
    resourceInfo.writeResources = m_resourceSync.writeResources;

    m_lastTicket = m_device->Submit(submitInfo, resourceInfo);


    if (m_isTransient) m_device->RegisterTransientCommandList(shared_from_this());
}
template <class> inline constexpr bool always_false = false;

void VkCommandList::Compile()
{
    if (m_recorded)
        return;

    assert(m_commandBuffer != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    m_expectedInitialLayouts.clear();

    HL_EXCEPTION(vkBeginCommandBuffer(m_commandBuffer, &beginInfo) !=
                     VK_SUCCESS,
                 "Failed to begin recording command buffer!");

    std::optional<RenderPassBeginInfo> currentRenderPassInfo;
    std::optional<VkPipelineLayout> currentPipelineLayout;
    Ref<VkGraphicsPipeline> currentPipeline;
    using Set = std::uint32_t;
    using Dirty = bool;
    std::map<Set, std::pair<Dirty, ResourceBindingMap>>
        lastBoundResourceMaps; // needs to be in order
    bool lastBoundResourceMapsChanged = true;
    std::unordered_set<Set> boundSets; // for safety check to prevent gpu crash

    Ref<UnsafeVertexBuffer> currentVertexBuffer;
    Ref<IndexBuffer> currentIndexBuffer;

    for (auto& command : m_commands)
    {
        std::visit(
            [&](auto&& cmd)
            {
                using T = std::decay_t<decltype(cmd)>;

                if constexpr (std::is_same_v<T, VkCommandBeginPass>)
                {
                    auto passKey = VkRenderPassCacheKey(cmd.beginInfo);

                    auto& renderPass = m_device->GetRenderPassCache().Acquire(passKey);
                    auto size = cmd.beginInfo.attachmentSpecifications[0].buffer->GetSize();

                    std::vector<::VkImageView> images;
                    images.reserve(cmd.beginInfo.attachmentSpecifications.size());
                    VkFramebufferCacheKey vkfbck{};

                    for (auto& attach : cmd.beginInfo.attachmentSpecifications)
                    {
                        auto img = std::static_pointer_cast<VkImage>(attach.buffer);
                        images.push_back(img->GetViewImage());
                        if (attach.loadOp == RenderPassBeginInfo::LoadOp::Load) m_expectedInitialLayouts[img] = img->GetDefaultLayout();
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
                    rpBegin.renderArea.offset = {0, 0};
                    rpBegin.renderArea.extent = {
                        .width = size.width,
                        .height = size.height,
                    };  

                    rpBegin.clearValueCount = static_cast<uint32_t>(cmd.clearValues.size());
                    rpBegin.pClearValues = cmd.clearValues.data();

                    HL_EXCEPTION(rpBegin.framebuffer == VK_NULL_HANDLE, "Frame buffer is null");
                    HL_EXCEPTION(rpBegin.renderPass == VK_NULL_HANDLE, "Render pass is null");
                    vkCmdBeginRenderPass(m_commandBuffer, &rpBegin,
                                         VK_SUBPASS_CONTENTS_INLINE);

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

                    for (auto& attach : cmd.beginInfo.attachmentSpecifications)
                    {
                        // // Must be in exact order!
                        // std::static_pointer_cast<VkImage>(attach.buffer)
                        //     ->GetSync()
                        //     .AppendWriteSemaphore(m_waitSemaphores);
                        // std::cout
                        //     << "BEFORE: "
                        //     << std::static_pointer_cast<VkImage>(attach.buffer)
                        //            ->GetSync()
                        //            .m_writeSemaphore
                        //     << std::endl;
                        // std::static_pointer_cast<VkImage>(attach.buffer)
                        //     ->GetSync()
                        //     .SetWrite(m_sync.semaphore, m_sync.fence);
                        // std::cout
                        //     << "AFTER: "
                        //     << std::static_pointer_cast<VkImage>(attach.buffer)
                        //            ->GetSync()
                        //            .m_writeSemaphore
                        //     << std::endl;
                        m_resourceSync.AddWrite(std::static_pointer_cast<VkImage>(attach.buffer));
                    }
                    currentRenderPassInfo = cmd.beginInfo;
                }
                else if constexpr (std::is_same_v<T, VkCommandEndPass>)
                {
                    vkCmdEndRenderPass(m_commandBuffer);
                    currentRenderPassInfo.reset();
                }
                else if constexpr (std::is_same_v<T, VkCommandBindPipeline>)
                {
                    if (!currentRenderPassInfo)
                    {
                        HL_MESSAGE("BindPipeline command outside a render "
                                   "pass, skipping.");
                        return;
                    }

                    auto& rpInfo = *currentRenderPassInfo;
                    auto prevPipeline = currentPipeline;
                    currentPipeline =
                        m_device->GetGraphicsPipelineCache().Acquire(
                            rpInfo, 0,
                            {
                                .vertexShaderModules = cmd.vertexShaderModules,
                                .pixelShaderModules = cmd.pixelShaderModules,
                                .geometryShaderModules = cmd.geometryShaderModules,
                                .topology = cmd.topology,
                                .cullMode = cmd.cullMode,
                                .fillMode = cmd.fillMode,
                            });
                    if (currentPipeline == nullptr)
                        return;
                    if (prevPipeline == currentPipeline)
                        return;

                    assert(currentPipeline->m_pipeline != VK_NULL_HANDLE);
                    vkCmdBindPipeline(m_commandBuffer,
                                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                                      currentPipeline->m_pipeline);
                    currentPipelineLayout = currentPipeline->m_pipelineLayout;
                    // lastBoundResourceMaps.clear();
                    for (auto& [set, layout] : currentPipeline->m_mergedBindingLayouts)
                    {
                        if (layout.m_bindings.empty()) continue;

                        for (auto& binding : layout.m_bindings)
                        {
                            if (lastBoundResourceMaps[set].second.bindings.contains(binding.type))
                            {
                                auto& bindingMap = lastBoundResourceMaps[set].second.bindings[binding.type];
                                if (bindingMap.find(binding.slot) != bindingMap.end()) continue;
                            }
                            lastBoundResourceMaps[set].first = true; // Prewarm to let it auto prefill for missing binds IMPORTANT!!! Removing this leads to GPU Driver Crash
                            break;
                        }
                    }

                    lastBoundResourceMapsChanged = true;
                }
                else if constexpr (std::is_same_v<T, VkCommandBindVertexBuffers>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    std::vector<VkBuffer> vkBuffers;
                    std::vector<VkDeviceSize> offsets;

                    vkBuffers.resize(cmd.buffers.size());
                    offsets.resize(cmd.buffers.size());

                    for (std::size_t i = 0, n = cmd.buffers.size(); i < n; ++i)
                    {
                        auto& bindInfo = cmd.buffers[i];
                        auto buff = std::static_pointer_cast<VkVertexBuffer>(bindInfo.buffer);
                        vkBuffers[i] = buff->GetBuffer();
                        offsets[i] = bindInfo.offset;

                        m_resourceSync.AddRead(std::static_pointer_cast<IVkSyncableResource>(buff));
                    }

                    assert(cmd.buffers.size() > 0);
                    assert(vkBuffers.size() == cmd.buffers.size());
                    assert(offsets.size() == cmd.buffers.size());
                    
                    constexpr int FIRST_BINDING = 0;
                    
                    if (cmd.buffers.size() > 0)
                        currentVertexBuffer = cmd.buffers[0].buffer;

                    vkCmdBindVertexBuffers(m_commandBuffer, FIRST_BINDING,
                                           cmd.buffers.size(), vkBuffers.data(),
                                           offsets.data());
                }
                else if constexpr (std::is_same_v<T, VkCommandBindIndexBuffer>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    vkCmdBindIndexBuffer(m_commandBuffer,
                                         cmd.buffer->GetBuffer(), cmd.offset,
                                         VK_INDEX_TYPE_UINT32);

                    currentIndexBuffer = cmd.buffer;

                    m_resourceSync.AddRead(std::static_pointer_cast<VkIndexBuffer>(cmd.buffer));
                }
                else if constexpr (std::is_same_v<T,
                                                  VkCommandBindUniformBuffer>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    assert(currentPipelineLayout.has_value());

                    if (!currentPipeline->HasBinding(cmd.module, cmd.slot, BindingType::UniformBuffer))
                        return; // Do not error, just skip binding. Shader compiler may optimize it out in some cases.

                    BIND_ERROR(
                        !currentPipeline->IsValid(cmd.module, cmd.slot,
                                                  BindingType::UniformBuffer),
                        std::format("The the binding in slot ({}) does not match a resource of "
                                    "type (UniformBuffer)",
                                    cmd.slot)
                            .c_str());

                    for (auto& set :
                         currentPipeline->GetSetsForModule(cmd.module))
                    {
                        lastBoundResourceMaps[set].first = true;
                        lastBoundResourceMaps[set].second.SetResource(
                            cmd.slot, cmd.buffer);

                        m_resourceSync.AddRead(std::static_pointer_cast<VkUniformBuffer>(cmd.buffer));
                    }
                    lastBoundResourceMapsChanged = true;
                }
                else if constexpr (std::is_same_v<T, VkCommandBindImage>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    assert(currentPipelineLayout.has_value());

                    if (!currentPipeline->HasBinding(cmd.module, cmd.slot, BindingType::Image))
                        return;

                    BIND_ERROR(
                        !currentPipeline->IsValid(cmd.module, cmd.slot,
                                                  BindingType::Image),
                        std::format("The binding in slot ({}) does not match a resource of "
                                    "type (Texture2D/Image)",
                                    cmd.slot)
                            .c_str());

                    for (auto& set :
                         currentPipeline->GetSetsForModule(cmd.module))
                    {
                        lastBoundResourceMaps[set].first = true;
                        lastBoundResourceMaps[set].second.SetResource(
                            cmd.slot, cmd.buffer);

                        m_expectedInitialLayouts[std::static_pointer_cast<VkImage>(cmd.buffer)] = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        m_resourceSync.AddRead(std::static_pointer_cast<VkImage>(cmd.buffer));
                    }
                    lastBoundResourceMapsChanged = true;
                }
                else if constexpr (std::is_same_v<T, VkCommandBindSampler>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    assert(currentPipelineLayout.has_value());

                    if (!currentPipeline->HasBinding(cmd.module, cmd.slot, BindingType::Sampler))
                        return;

                    BIND_ERROR(
                        !currentPipeline->IsValid(cmd.module, cmd.slot,
                                                  BindingType::Sampler),
                        std::format("The binding in slot ({}) does not match a resource of "
                                    "type (Sampler)",
                                    cmd.slot)
                            .c_str());

                    for (auto& set :
                         currentPipeline->GetSetsForModule(cmd.module))
                    {
                        lastBoundResourceMaps[set].first = true;
                        lastBoundResourceMaps[set].second.SetResource(
                            cmd.slot, cmd.sampler);
                    }
                    lastBoundResourceMapsChanged = true;
                }
                else if constexpr (std::is_same_v<T, VkCommandDraw>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    for (auto& [set, lay] :
                         currentPipeline->m_mergedBindingLayouts)
                        BIND_ERROR(
                            !lay.m_bindings.empty() && !boundSets.contains(set),
                            std::format(
                                "Set {} is missing from currently bound sets!",
                                set)
                                .c_str());

                    vkCmdDraw(m_commandBuffer, cmd.vertexCount,
                              cmd.instanceCount, cmd.firstVertex,
                              cmd.firstInstance);
                }
                else if constexpr (std::is_same_v<T, VkCommandDrawIndexed>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    

                    for (auto& [set, lay] :
                         currentPipeline->m_mergedBindingLayouts)
                        BIND_ERROR(
                            !lay.m_bindings.empty() && !boundSets.contains(set),
                            std::format(
                                "Set {} is missing from currently bound sets!",
                                set)
                                .c_str());

                    if (currentIndexBuffer == nullptr)
                        return;

                    vkCmdDrawIndexed(
                        m_commandBuffer, cmd.indexCount, cmd.instanceCount,
                        cmd.firstIndex, cmd.vertexOffset, cmd.firstInstance
                    );
                }
                else if constexpr (std::is_same_v<T, VkCommandBindLayouts>)
                {
                    if (currentPipeline == nullptr)
                        return;

                    assert(currentPipelineLayout.has_value());
                    if (!lastBoundResourceMapsChanged)
                        return;

                    std::vector<VkDescriptorSet> descSets;

                    auto bindSet = [&](std::uint32_t first)
                    {
                        vkCmdBindDescriptorSets(
                            m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            currentPipelineLayout.value(), first,
                            descSets.size(), descSets.data(), 0, nullptr);
                    };

                    std::uint32_t batchStart = 0u;
                    for (auto& [set, value] : lastBoundResourceMaps)
                    {
                        auto& [dirty, resourceMap] = value;

                        if (dirty)
                        {
                            VkDescriptorSetKey tmp = {
                                .layout =
                                    currentPipeline->m_mergedBindingLayouts.at(
                                        set),
                                .bindingMap = resourceMap,
                            };

                            auto& layoutWrapper =
                                currentPipeline->m_layouts[set];
                            auto x = layoutWrapper->Get();
                            auto y = m_device->GetDescriptorSetCache().Acquire(
                                tmp, x);
                            if (y == nullptr)
                                continue;

                            if (descSets.empty())
                                batchStart = set;
                            descSets.push_back(*y);
                            boundSets.insert(set);
                            dirty = false;
                        }
                        else if (!descSets.empty())
                        {
                            bindSet(batchStart);
                            descSets.clear();
                        }
                    }

                    if (descSets.size() > 0)
                        bindSet(batchStart);
                    lastBoundResourceMapsChanged = false;
                }
                else
                {
                    static_assert(always_false<T>,
                                  "Unknown command type in command list");
                }
            },
            command);
    }

    HL_EXCEPTION(vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS, "Failed to end command buffer");

    //normalize resources
    m_resourceSync.Normalize();
    
    m_recorded = true;
}

void VkCommandList::TransitionLayout(Ref<VkImage> image,
                                     VkImageLayout oldLayout,
                                     VkImageLayout newLayout)
{
    if (oldLayout == newLayout)
        return;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = std::static_pointer_cast<VkImage>(image)->GetImage();

    if (image->IsDepth())
        barrier.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_DEPTH_BIT |
            VK_IMAGE_ASPECT_STENCIL_BIT; // TODO: Check if stencil is actually present
    else
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else
    {
        // Fallback for other transitions (generic)
        barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        barrier.dstAccessMask =
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }

    vkCmdPipelineBarrier(m_commandBuffer, sourceStage, destinationStage, 0, 0,
                         nullptr, 0, nullptr, 1, &barrier);
}

// void VkCommandList::TransitionLayout(Ref<VkTexture2D> texture,
//                                      VkImageLayout oldLayout,
//                                      VkImageLayout newLayout)
// {
//     std::cout << "TransitionLayout: image=" << texture->GetImage() 
//               << " from=" << oldLayout << " to=" << newLayout << std::endl;
    
//     if (oldLayout == newLayout)
//         return;

//     VkImageMemoryBarrier barrier{};
//     barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//     barrier.oldLayout = oldLayout;
//     barrier.newLayout = newLayout;
//     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//     barrier.image = texture->GetImage();

//     barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//     barrier.subresourceRange.baseMipLevel = 0;
//     barrier.subresourceRange.levelCount = 1;
//     barrier.subresourceRange.baseArrayLayer = 0;
//     barrier.subresourceRange.layerCount = 1;

//     VkPipelineStageFlags sourceStage;
//     VkPipelineStageFlags destinationStage;

//     if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
//         newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//     {
//         barrier.srcAccessMask = 0;
//         barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//         sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//         destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//     }
//     else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
//              newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//     {
//         barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//         barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//         sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//         destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//     }
//     else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
//              newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//     {
//         barrier.srcAccessMask = 0;
//         barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//         sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//         destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//     }
//     else
//     {
//         barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
//         barrier.dstAccessMask =
//             VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
//         sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
//         destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
//     }

//     vkCmdPipelineBarrier(m_commandBuffer, sourceStage, destinationStage, 0, 0,
//                          nullptr, 0, nullptr, 1, &barrier);
// }

void VkCommandList::BeginRenderPass(const RenderPassBeginInfo& renderPassInfo)
{
    if (IsRecorded(m_recorded))
        return;

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
                auto& clearDepthStencil =
                    attachmentSpec.clearDepthStencil.value();
                clearValues.emplace_back(VkClearValue{
                    .depthStencil{.depth = clearDepthStencil.depth,
                                  .stencil = clearDepthStencil.stencil}});
            }
            else
                clearValues.emplace_back(
                    VkClearValue{.color = {{0.f, 0.f, 0.f, 1.f}}});
        }
        else
        {
            if (attachmentSpec.clearColor.has_value())
            {
                auto& color = attachmentSpec.clearColor.value();
                clearValues.emplace_back(VkClearValue{
                    .color = {{color.r, color.g, color.b, color.a}}});
            }
            else
            {
                clearValues.emplace_back(
                    VkClearValue{.color = {{0.f, 0.f, 0.f, 1.f}}});
            }
        }
    }

    m_commands.emplace_back(VkCommandBeginPass{.beginInfo = renderPassInfo,
                                               .clearValues = clearValues});

    m_in_renderPass = true;
}
void VkCommandList::EndRenderPass()
{
    if (IsRecorded(m_recorded))
        return;

    if (!m_in_renderPass)
    {
        HL_MESSAGE("RenderPass was not started!");
        return;
    }

    m_commands.emplace_back(VkCommandEndPass{});

    m_in_renderPass = false;
    m_lastBoundPipelineInputLayouts.reset();
    // m_lastBoundPipelineIndex.reset();
}

void VkCommandList::BindPipeline(const BindPipelineInfo& pipelineInfo)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    
    BIND_ERROR(pipelineInfo.vertexShaderModules.empty(), "Pipeline must contain at least 1 Vertex Module!");
    BIND_ERROR(pipelineInfo.pixelShaderModules.empty(), "Pipeline must contain at least 1 Pixel Module!");

    auto HasMain = [](const std::vector<Ref<ShaderModule>>& mods)
    {
        return std::find_if(
            mods.begin(), mods.end(), [](Ref<ShaderModule> x)
            { return x->GetFamily() == ShaderModule::Family::Main; });
    };

    auto vsMain = HasMain(pipelineInfo.vertexShaderModules);
    BIND_ERROR(
        vsMain == pipelineInfo.vertexShaderModules.end(),
        "Pipeline must contain at least 1 Vertex Module that is of Family::Main!");
    BIND_ERROR(
        HasMain(pipelineInfo.pixelShaderModules) == pipelineInfo.vertexShaderModules.end(),
        "Pipeline must contain at least 1 Pixel Module that is of Family::Main!"
    );
    if (!pipelineInfo.geometryShaderModules.empty())
    {
        BIND_ERROR(
            HasMain(pipelineInfo.geometryShaderModules) == pipelineInfo.vertexShaderModules.end(),
            "Pipeline must contain at least 1 Geometry Module that is of Family::Main!"
        );
    }


    // verify against last bind
    if (m_lastBoundPipelineInputLayouts.has_value())
    {
        auto vsInputLayouts = vsMain->get()->GetInputLayouts();
        if (vsInputLayouts.has_value())
        {
            const auto& newLayouts = vsInputLayouts->GetLayouts();
            const auto& oldLayouts = m_lastBoundPipelineInputLayouts->GetLayouts();

            BIND_ERROR(newLayouts.size() > 32, "Too many vertex buffer layouts! Max is 32!"); // cuz uint32 is 32bit so allows 32 bools
            // i mean who tf needs more than 32 vertex buffers bound at once???

            for (uint32_t i = 0; i < newLayouts.size(); ++i)
            {
                if (!(m_vbValidMask & (1 << i))) continue; // if the bit is not set we skip it since its already 0

                // since i already store prev layouts cuz i use it for validation
                // i can use it for that here too
                // so i first check if it has anything set to it and if it does
                // i check if it matches the new layout
                // if it doesn't match i invalidate it
                if (m_boundVbLayouts[i].has_value() && !(newLayouts[i] == *m_boundVbLayouts[i]))
                {
                    m_vbValidMask &= ~(1 << i); // same as above set it to 0 ~ is inverse so i get the inverse of i meaning i set everything else to 1 while setting to 0 the bit we need to set to 0
                }
            }
        }
    }

    m_lastBoundPipelineIndex = m_commands.size();
    m_lastBoundPipelineInputLayouts = vsMain->get()->GetInputLayouts().value(); // On module creation its ensured that inputlayouts always exists!


    auto vertexShader = m_device->GetShaderCache().Acquire<VertexShader>(pipelineInfo.vertexShaderModules);
    auto pixelShader = m_device->GetShaderCache().Acquire<PixelShader>(pipelineInfo.pixelShaderModules);

    Ref<GeometryShader> geometryShader;
    if (!pipelineInfo.geometryShaderModules.empty())
    {
        geometryShader = m_device->GetShaderCache().Acquire<GeometryShader>(pipelineInfo.geometryShaderModules);
    }

    m_commands.emplace_back(VkCommandBindPipeline{
        .vertexShaderModules = pipelineInfo.vertexShaderModules,
        .pixelShaderModules = pipelineInfo.pixelShaderModules,
        .geometryShaderModules = pipelineInfo.geometryShaderModules,
        .vertexShader = vertexShader,
        .pixelShader = pixelShader,
        .geometryShader = geometryShader,
        .topology = pipelineInfo.topology,
        .cullMode = pipelineInfo.cullMode,
        .fillMode = pipelineInfo.fillMode,
    });
}

void VkCommandList::BindUniformBuffer(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<UnsafeUniformBuffer>& buffer)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    m_commands.emplace_back(VkCommandBindUniformBuffer{
        .module = module,
        .slot = slot,
        .buffer = buffer,
    });
}

void VkCommandList::BindTexture2D(const Ref<ShaderModule>& module, std::uint32_t slot, const Ref<Texture2D>& buffer)
{
    BindImage(module, slot, buffer->GetImage());
}

void VkCommandList::BindImage(const Ref<ShaderModule>& module,
                                  std::uint32_t slot,
                                  const Ref<Image>& buffer)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    auto vkImage = std::static_pointer_cast<VkImage>(buffer);

    m_commands.emplace_back(VkCommandBindImage{
        .module = module,
        .slot = slot,
        .buffer = buffer,
    });

    std::cout << "Binding image " << vkImage->GetImage() << " to slot " << slot
              << std::endl;
}

void VkCommandList::BindSampler(const Ref<ShaderModule>& module,
                                std::uint32_t slot, const Ref<Sampler>& sampler)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    m_commands.emplace_back(VkCommandBindSampler{
        .module = module,
        .slot = slot,
        .sampler = sampler,
    });
}

void ForEachStage(auto& pipeline, std::string_view name, std::function<void(BindingLink::Binding, Ref<ShaderModule>&)> func)
{
    std::visit([pipeline, name, func](auto& pipelinetmp)
    {
        if constexpr (std::is_same_v<std::remove_reference_t<decltype(pipelinetmp)>, VkCommandBindPipeline>)
        {
            auto& vertModule = pipelinetmp.vertexShader;
            auto& pixelModule = pipelinetmp.pixelShader;
            auto& geometryModule = pipelinetmp.geometryShader;

            if (vertModule)
            {
                auto binding = vertModule->GetBindingLink().GetBinding(name);
                if (binding)
                {
                    func(*binding, pipelinetmp.vertexShaderModules[binding->set]);
                    return;
                }
            }
            if (pixelModule)
            {
                auto binding = pixelModule->GetBindingLink().GetBinding(name);
                if (binding)
                {
                    func(*binding, pipelinetmp.pixelShaderModules[binding->set]);
                    return;
                }
            }
            if (geometryModule)
            {
                auto binding = geometryModule->GetBindingLink().GetBinding(name);
                if (binding)
                {
                    func(*binding, pipelinetmp.geometryShaderModules[binding->set]);
                    return;
                }
            }
        }
    }, pipeline);
}

void VkCommandList::BindUniformBuffer(std::string_view name, const Ref<UnsafeUniformBuffer>& buffer)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    auto& pipeline = m_commands[m_lastBoundPipelineIndex.value()];

    bool found = false;
    

    ForEachStage(pipeline, name, [&](BindingLink::Binding binding, Ref<ShaderModule> module)
    {
        BindUniformBuffer(module, binding.slot, buffer);
        found = true;
    });

    BIND_ERROR(!found, std::format("Binding {} not found!", name).c_str());
}

void VkCommandList::BindTexture2D(std::string_view name, const Ref<Texture2D>& texture)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    BIND_ERROR((texture->GetImage()->GetUsage() & Image::Usage::Sampled) != Image::Usage::Sampled, "This texture cannot be sampled!");

    auto& pipeline = m_commands[m_lastBoundPipelineIndex.value()];

    bool found = false;
    
    ForEachStage(pipeline, name, [&](BindingLink::Binding binding, Ref<ShaderModule> module)
    {
        BindTexture2D(module, binding.slot, texture);
        found = true;
    });

    BIND_ERROR(!found, std::format("Binding {} not found!", name).c_str());
}

void VkCommandList::BindImage(std::string_view name, const Ref<Image>& image)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");\

    BIND_ERROR((image->GetUsage() & Image::Usage::Sampled) != Image::Usage::Sampled, "This image cannot be sampled!");

    auto& pipeline = m_commands[m_lastBoundPipelineIndex.value()];

    bool found = false;
    
    ForEachStage(pipeline, name, [&](BindingLink::Binding binding, Ref<ShaderModule> module)
    {
        BindImage(module, binding.slot, image);
        found = true;
    });

    BIND_ERROR(!found, std::format("Binding {} not found!", name).c_str());
}

void VkCommandList::BindSampler(std::string_view name, const Ref<Sampler>& sampler)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    auto& pipeline = m_commands[m_lastBoundPipelineIndex.value()];

    bool found = false;
    
    ForEachStage(pipeline, name, [&](BindingLink::Binding binding, Ref<ShaderModule> module)
    {
        BindSampler(module, binding.slot, sampler);
        found = true;
    });

    BIND_ERROR(!found, std::format("Binding {} not found!", name).c_str());
}

void VkCommandList::BindVertexBuffers(std::initializer_list<Ref<UnsafeVertexBuffer>> buffers)
{
    std::vector<VertexBufferBindInfo> bindInfos;
    bindInfos.reserve(buffers.size());
    for (auto& buffer : buffers)
    {
        bindInfos.emplace_back(buffer, 0);
    }
    
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    BIND_ERROR(bindInfos.size() != m_lastBoundPipelineInputLayouts->size(), 
               std::format("VertexBuffers must match the input layout used by the VertexShader! Missatching sizes: {} != {}", 
               bindInfos.size(), m_lastBoundPipelineInputLayouts->size()).c_str());

    const auto& pipelineLayouts = m_lastBoundPipelineInputLayouts->GetLayouts();

    for (std::size_t i = 0, n = bindInfos.size(); i < n; ++i)
    {
        auto& bindInfo = bindInfos[i];
        uint32_t stride = pipelineLayouts[i].GetStride();
        
        BIND_ERROR(bindInfo.buffer->GetSize() < bindInfo.offset + stride,
                   std::format("VertexBuffer at index {} is too small for the expected stride! Size: {}, Offset: {}, Stride: {}", 
                   i, bindInfo.buffer->GetSize(), bindInfo.offset, stride).c_str());

        m_vbValidMask |= (1 << i);
        m_boundVbLayouts[i] = pipelineLayouts[i];
    }

    m_commands.emplace_back(VkCommandBindVertexBuffers{.buffers = std::move(bindInfos)});
}

void VkCommandList::BindVertexBuffers(std::initializer_list<VertexBufferBindInfo> buffers)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    BIND_ERROR(
        buffers.size() != m_lastBoundPipelineInputLayouts->size(), 
        std::format("VertexBuffers must match the input layout used by the VertexShader! Missatching sizes: {} != {}",
        buffers.size(), m_lastBoundPipelineInputLayouts->size()).c_str()
    );

    const auto& pipelineLayouts = m_lastBoundPipelineInputLayouts->GetLayouts();

    for (std::size_t i = 0, n = buffers.size(); i < n; ++i)
    {
        const auto& bindInfo = *(buffers.begin() + i);
        uint32_t stride = pipelineLayouts[i].GetStride();
        
        BIND_ERROR(
            bindInfo.buffer->GetSize() < bindInfo.offset + stride,
            std::format("VertexBuffer at index {} is too small for the expected stride! Size: {}, Offset: {}, Stride: {}", 
            i, bindInfo.buffer->GetSize(), bindInfo.offset, stride).c_str()
        );

        m_vbValidMask |= (1 << i);
        m_boundVbLayouts[i] = pipelineLayouts[i];
    }

    std::vector<VertexBufferBindInfo> vBuffers(buffers);
    m_commands.emplace_back(VkCommandBindVertexBuffers{.buffers = std::move(vBuffers)});
}

void VkCommandList::BindIndexBuffer(const Ref<IndexBuffer>& buffer,
                                    std::uint32_t offset)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    currentIndexBuffer = buffer;
    currentIndexBufferOffset = offset;

    m_commands.emplace_back(VkCommandBindIndexBuffer{
        .buffer = std::static_pointer_cast<VkIndexBuffer>(buffer),
        .offset = offset});
}

void VkCommandList::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    BIND_ERROR(!m_lastBoundPipelineInputLayouts.has_value(), "No pipeline bound!");

    // because 1 << size tells it to move the first bit to the size of the layouts vector
    // and since that will always result in the highest after value set for that max bit size okay what am i saying
    // point is for example 7 is 111 so for 1 << 3 we get 8 since 1000 then if we subtract by 1 we get 7 which is 111
    // anyways yeah basic bitwise bs
    // and since later we are doing masking stuff
    // we check is the mask encompasing our validation mask (our progress during binding)
    // which also makes it so it only checks the mask amount since
    // from digital electronics we know that 1 & 1 = 1 and 1 & 0 = 0
    // meaning if we combine required mask and our mask
    // we eliminate all traliling bits which removes issue of reading bits that we dont need to check
    // i like this cuz idk this solution is cooler???
    // anyways point is since you eliminate the trailing you are left with the exact size
    // and since we know the size which is requiredmask we just check if its equal, in this case
    // if its not equal so we can error if not
    // anyways this is just masking principle behind masks like for example layer mask or even permissions in discord api
    // DZ OUT
    uint32_t requiredMask = (1 << m_lastBoundPipelineInputLayouts->size()) - 1;
    BIND_ERROR(
        (m_vbValidMask & requiredMask) != requiredMask,
        "One or more vertex buffer slots are empty or incompatible with the current pipeline layout!"
    );

    m_commands.emplace_back(VkCommandBindLayouts{});
    m_commands.emplace_back(VkCommandDraw{
        .vertexCount = vertexCount,
        .instanceCount = 1,
        .firstVertex = firstVertex,
        .firstInstance = 0,
    });
}

void VkCommandList::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex, std::int32_t vertexOffset, std::uint32_t firstInstance)
{
    if (IsRecorded(m_recorded))
        return;

    BIND_ERROR(!m_in_renderPass, "RenderPass was not started! Can't bind pipeline outside render pass.");
    BIND_ERROR(!m_lastBoundPipelineIndex.has_value(), "No bound pipeline! Can't bind resource outside pipeline.");

    BIND_ERROR(!m_lastBoundPipelineInputLayouts.has_value(), "No pipeline bound!");
    
    
    BIND_ERROR(currentIndexBuffer == nullptr, "No index buffer bound!");
    
    BIND_ERROR(currentIndexBuffer->GetCount() < indexCount + firstIndex, "IndexBuffer is too small for the expected index count!");
    // TODO: Check for offset overflow

    uint32_t requiredMask = (1 << m_lastBoundPipelineInputLayouts->size()) - 1;
    BIND_ERROR(
        (m_vbValidMask & requiredMask) != requiredMask,
        "One or more vertex buffer slots are empty or incompatible with the current pipeline layout!"
    );
    

    m_commands.emplace_back(VkCommandBindLayouts{});
    m_commands.emplace_back(VkCommandDrawIndexed{
        .indexCount = indexCount,
        .instanceCount = instanceCount,
        .firstIndex = firstIndex,
        .firstInstance = firstInstance,
        .vertexOffset = vertexOffset,
    });
}

} // namespace Helios::Internal