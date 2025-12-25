#pragma once

#include "pch.h"
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{

    constexpr uint8 MAX_FRAMES_UNTIL_DECAY = 5;
    constexpr uint32 MAX_RECYCLE_CHECKS = 10;
    constexpr uint32 MAX_SYNC_OBJECTS = 1 << 31;
    constexpr uint32 MAX_RECYCLE_OBJECTS_THRESHOLD = 10;

    constexpr uint8 EXTEND_SIZE = 5;
    constexpr uint32 GENERATION_MASK = 0xFFFFFFFF;
    constexpr uint32 INDEX_MASK = 0x7FFFFFFF;
    constexpr uint32 AVAILABLE_MASK = 0x80000000;

    struct SyncTicket
    {
        union
        {
            struct
            {
                uint32 generation : 32;
                uint32 index : 31;
                uint32 isAvailable : 1;
            };
            uint64 id;
        };

        SyncTicket() : id(0) {}
        SyncTicket(uint64 id) : id(id) {}
        SyncTicket(uint32 generation, uint32 index) : generation(generation), index(index), isAvailable(false) {}
        
        bool operator==(const SyncTicket& other) const { return id == other.id; }
    };

    struct SyncObject
    {
        VkFence fence = VK_NULL_HANDLE;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        bool fenceSignaled = false;
        bool semaphoreConsumed = false;
        bool isActive = false;
        uint32 generation = 0u;
        uint8 framesAgo = 0u;
    };

    struct SubmitWait
    {
        SyncTicket ticket;
        VkPipelineStageFlags stage;
    };

    struct ResolvedSubmitWait
    {
        SyncTicket ticket;
        VkFence submitFence;
        VkSemaphore submitSemaphore;
        uint64_t signalTimelineValue = 0;
        std::vector<VkSemaphore> binarySemaphores;
        std::vector<VkPipelineStageFlags> stages;
        std::vector<VkSemaphoreSubmitInfo> newSemaphores;
    };

    struct VkSyncPoolStats
    {
        uint32_t acquiredThisFrame = 0;
        uint32_t retiredThisFrame = 0;
        uint32_t activeCount = 0;
        uint32_t freeCount = 0;
        uint32_t deadCount = 0;
        uint32_t fencePoolSize = 0;
        uint32_t semaphorePoolSize = 0;
        uint32_t totalCount = 0;
        uint32_t capacity = 0;
    };
    
    class VkGraphicsDevice;
	class VkSyncPool
	{
	public:
		VkSyncPool(Ref<VkGraphicsDevice> device);

        ~VkSyncPool();
	
        SyncTicket Acquire();

        void Step();

        ResolvedSubmitWait ResolveSubmitWait(const std::vector<SubmitWait>& submitWaits);

        void Wait(const std::vector<SyncTicket>& syncTickets);
        void Wait(SyncTicket ticket);

        bool IsInFlight(SyncTicket ticket);
        bool IsInFlight(const std::vector<SyncTicket>& syncTickets);

        // Just check if ticket is expired or not
        bool IsValid(SyncTicket ticket);

        size_t GetRecycleCount() const { return m_freeIndices.size(); }
        size_t GetSize() const { return m_syncObjectCount; }
        size_t GetCapacity() const { return m_syncObjectCapacity; } 

        const VkSyncPoolStats& GetStats() const { return m_finalizedStats; }
        std::string GetStatsString() const;
        void SetStatsEnabled(bool enabled) { m_enableStats = enabled; }
        bool IsStatsEnabled() const { return m_enableStats; }

	private:

        std::vector<uint32> m_freeIndices;
        std::vector<uint32> m_deadIndices;
        
        std::vector<VkFence> m_fencePool;
        std::vector<VkSemaphore> m_semaphorePool;

        size_t m_syncRecycleCount = 0;

        void Extend();

		Ref<VkGraphicsDevice> m_device;
        mutable std::recursive_mutex m_mutex;

        size_t m_syncObjectCapacity = 0;
        size_t m_syncObjectCount = 0;
        SyncObject* m_syncObjects = NULL;

        VkSyncPoolStats m_currentStats;
        VkSyncPoolStats m_finalizedStats;
        uint64_t m_lastFrameCount = 0;
        bool m_enableStats = false;
	};
}