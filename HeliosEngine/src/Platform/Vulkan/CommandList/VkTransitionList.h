#pragma once

#include "Platform/Vulkan/Sync/VkSyncPool.h"
#include "Platform/Vulkan/VkImage.h"


namespace Helios::Internal
{
    class VkTransitionList final
    {
      public:
        VkTransitionList(const Ref<VkGraphicsDevice>& device,
                         VkCommandPool commandPool);
        ~VkTransitionList();

        void Reset();
        void Execute(const std::vector<VkSemaphore>& waitSemaphores);

        void TransitionLayout(Ref<VkImage> image, VkImageLayout newLayout);
        SyncTicket GetTicket() { return m_ticket; }

      private:
        Ref<VkGraphicsDevice> m_device;
        VkCommandBuffer m_commandBuffer;
        VkCommandPool m_commandPool;

        SyncTicket m_ticket;
        std::vector<Ref<VkImage>> m_transitionedImages;
    };
} // namespace Helios::Internal