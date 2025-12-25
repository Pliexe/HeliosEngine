#include "VkTransitionList.h"
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    VkTransitionList::VkTransitionList(const Ref<VkGraphicsDevice>& device,
                                       VkCommandPool commandPool) : m_device(device), m_commandPool(commandPool)
    {

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        HL_EXCEPTION(vkAllocateCommandBuffers(*m_device, &allocInfo,
                                              &m_commandBuffer) != VK_SUCCESS,
                     "Failed to allocate command buffers!");
    }

    VkTransitionList::~VkTransitionList()
    {
        if (m_ticket.id != 0)
            m_device->GetSyncPool().Wait(m_ticket);
        
        vkFreeCommandBuffers(*m_device, m_commandPool, 1, &m_commandBuffer);
    }

    void VkTransitionList::Reset()
    {
        if (m_ticket.id != 0)
            m_device->GetSyncPool().Wait(m_ticket);

        vkResetCommandBuffer(m_commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(m_commandBuffer, &beginInfo);

        m_transitionedImages.clear();
        m_ticket = SyncTicket();
    }

    void
    VkTransitionList::Execute(const std::vector<VkSemaphore>& waitSemaphores)
    {
        vkEndCommandBuffer(m_commandBuffer);

        VkGraphicsDevice::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.commandBuffers = &m_commandBuffer;
        submitInfo.waitSemaphores = waitSemaphores;

        // Default to all wait stages being COLOR_ATTACHMENT_OUTPUT or TOP_OF_PIPE if not specified
        // For simplicity in TransitionList, we'll use TOP_OF_PIPE
        submitInfo.waitStages.resize(waitSemaphores.size(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

        VkGraphicsDevice::SubmitResourceInfo resInfo{};
        for (auto& image : m_transitionedImages)
            resInfo.writeResources.push_back(image);

        m_ticket = m_device->Submit(submitInfo, resInfo);
    }

    void VkTransitionList::TransitionLayout(Ref<VkImage> image,
                                            VkImageLayout newLayout)
    {
        VkImageLayout oldLayout = image->GetPendingLayout();

        if (oldLayout == newLayout)
            return;

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image->GetImage();
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
        else
        {
            // fallback
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        vkCmdPipelineBarrier(m_commandBuffer, sourceStage, destinationStage, 0,
                             0, nullptr, 0, nullptr, 1, &barrier);

        image->UpdatePendingLayout(newLayout);
        m_transitionedImages.push_back(image);
    }

} // namespace Helios::Internal