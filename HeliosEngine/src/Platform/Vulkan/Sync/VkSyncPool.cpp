#include "VkSyncPool.h"

#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Helios/Core/Exceptions.h"
#include <vulkan/vulkan_core.h>
#include <algorithm>

using namespace Helios::Internal;

VkSyncPool::VkSyncPool(Ref<VkGraphicsDevice> device) : m_device(device)
{
    m_syncObjectCapacity = Graphics::GetMaxFramesInFlight() * EXTEND_SIZE;
    HL_EXCEPTION(
        (m_syncObjects = (SyncObject*)malloc(m_syncObjectCapacity * sizeof(SyncObject))) == NULL,
        "Failed to allocate VkSyncPool! Out of memory!"
    );
}

VkSyncPool::~VkSyncPool()
{
    std::lock_guard lock(m_mutex);
    
    // Wait for all active fences
    std::vector<VkFence> fences;
    for (size_t i = 0; i < m_syncObjectCount; i++)
    {
        if (m_syncObjects[i].fence != VK_NULL_HANDLE)
            fences.push_back(m_syncObjects[i].fence);
    }
    
    // Also include fences in the pool
    fences.insert(fences.end(), m_fencePool.begin(), m_fencePool.end());

    if (!fences.empty())
        vkWaitForFences(*m_device, (uint32_t)fences.size(), fences.data(), true, UINT64_MAX);

    for (auto& fence : fences)
        vkDestroyFence(*m_device, fence, nullptr);

    // Destroy all semaphores (active and pooled)
    for (size_t i = 0; i < m_syncObjectCount; i++)
    {
        if (m_syncObjects[i].semaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(*m_device, m_syncObjects[i].semaphore, nullptr);
    }
    
    for (auto& sem : m_semaphorePool)
        vkDestroySemaphore(*m_device, sem, nullptr);
    
    free(m_syncObjects);
}

ResolvedSubmitWait VkSyncPool::ResolveSubmitWait(const std::vector<SubmitWait>& submitWaits)
{
    std::lock_guard lock(m_mutex);
    ResolvedSubmitWait resolved;
    resolved.ticket = Acquire();
    resolved.submitFence = m_syncObjects[resolved.ticket.index].fence;
    resolved.submitSemaphore = m_syncObjects[resolved.ticket.index].semaphore;

    struct SemaphoreWait
    {
        VkSemaphore semaphore;
        VkPipelineStageFlags stage;
        uint32_t index;
    };

    std::vector<SemaphoreWait> pairedWaits;
    pairedWaits.reserve(submitWaits.size());

    for (auto& wait : submitWaits)
    {
        if (wait.ticket.index >= m_syncObjectCount)
            continue;

        auto& syncObj = m_syncObjects[wait.ticket.index];
        
        if (syncObj.generation != wait.ticket.generation)
            continue;
        
        if (syncObj.semaphore != VK_NULL_HANDLE)
        {
            pairedWaits.push_back({ syncObj.semaphore, wait.stage, (uint32_t)wait.ticket.index });
        }
    }

    if (!pairedWaits.empty())
    {
        std::sort(pairedWaits.begin(), pairedWaits.end(), [](const SemaphoreWait& a, const SemaphoreWait& b) {
            return (uint64_t)a.semaphore < (uint64_t)b.semaphore;
        });

        resolved.binarySemaphores.reserve(pairedWaits.size());
        resolved.stages.reserve(pairedWaits.size());

        auto addWait = [&](const SemaphoreWait& sw) {
            resolved.binarySemaphores.push_back(sw.semaphore);
            resolved.stages.push_back(sw.stage);
            
            auto& obj = m_syncObjects[sw.index];
            obj.semaphoreConsumed = true;
            m_semaphorePool.push_back(obj.semaphore);
            obj.semaphore = VK_NULL_HANDLE;
        };

        addWait(pairedWaits[0]);

        for (size_t i = 1; i < pairedWaits.size(); i++)
        {
            if (pairedWaits[i].semaphore == resolved.binarySemaphores.back())
            {
                resolved.stages.back() |= pairedWaits[i].stage;
            }
            else
            {
                addWait(pairedWaits[i]);
            }
        }
    }

    return resolved;
}

void VkSyncPool::Wait(const std::vector<SyncTicket>& syncTickets)
{
    std::lock_guard lock(m_mutex);
    if (syncTickets.empty())
        return;

    std::vector<VkFence> fences;
    std::vector<uint32> indices;
    fences.reserve(syncTickets.size());
    indices.reserve(syncTickets.size());
    for (auto& ticket : syncTickets)
    {
        if (ticket.index >= m_syncObjectCount)
            continue;
        
        auto& obj = m_syncObjects[ticket.index];
        if (ticket.generation != obj.generation)
            continue;

        if (!obj.fenceSignaled && obj.fence != VK_NULL_HANDLE)
        {
            fences.push_back(obj.fence);
            indices.push_back((uint32_t)ticket.index);
        }
    }

    if (!fences.empty())
    {
        vkWaitForFences(*m_device, (uint32_t)fences.size(), fences.data(), true, UINT64_MAX);
        for (auto index : indices)
        {
            auto& obj = m_syncObjects[index];
            obj.fenceSignaled = true;
            m_fencePool.push_back(obj.fence);
            obj.fence = VK_NULL_HANDLE;
        }
    }
}

void VkSyncPool::Wait(SyncTicket ticket)
{
    std::lock_guard lock(m_mutex);
    if (ticket.index >= m_syncObjectCount)
        return;
    
    auto& obj = m_syncObjects[ticket.index];
    if (ticket.generation != obj.generation)
        return;

    if (obj.fenceSignaled)
        return;

    if (obj.fence != VK_NULL_HANDLE)
    {
        vkWaitForFences(*m_device, 1, &obj.fence, true, UINT64_MAX);
        obj.fenceSignaled = true;
        m_fencePool.push_back(obj.fence);
        obj.fence = VK_NULL_HANDLE;
    }
}

bool VkSyncPool::IsInFlight(SyncTicket ticket)
{
    std::lock_guard lock(m_mutex);
    if (ticket.index >= m_syncObjectCount)
        return false;

    auto& obj = m_syncObjects[ticket.index];
    if (ticket.generation != obj.generation)
        return false;

    if (!obj.fenceSignaled && obj.fence != VK_NULL_HANDLE)
    {
        if (vkGetFenceStatus(*m_device, obj.fence) == VK_SUCCESS)
        {
            obj.fenceSignaled = true;
            m_fencePool.push_back(obj.fence);
            obj.fence = VK_NULL_HANDLE;
        }
    }

    return !obj.fenceSignaled;
}

bool VkSyncPool::IsInFlight(const std::vector<SyncTicket>& syncTickets)
{
    std::lock_guard lock(m_mutex);
    for (const auto& ticket : syncTickets)
    {
        if (ticket.index >= m_syncObjectCount)
            continue;

        auto& obj = m_syncObjects[ticket.index];
        if (ticket.generation != obj.generation)
            continue;

        if (!obj.fenceSignaled && obj.fence != VK_NULL_HANDLE)
        {
            if (vkGetFenceStatus(*m_device, obj.fence) == VK_SUCCESS)
            {
                obj.fenceSignaled = true;
                m_fencePool.push_back(obj.fence);
                obj.fence = VK_NULL_HANDLE;
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}

bool VkSyncPool::IsValid(SyncTicket ticket)
{
    std::lock_guard lock(m_mutex);
    if (ticket.index >= m_syncObjectCount)
        return false;

    auto& obj = m_syncObjects[ticket.index];
    return ticket.generation == obj.generation;
}

SyncTicket VkSyncPool::Acquire()
{
    std::lock_guard lock(m_mutex);
    
    if (m_enableStats)
        m_currentStats.acquiredThisFrame++;

    if (++m_syncRecycleCount > MAX_RECYCLE_OBJECTS_THRESHOLD)
    {
        m_syncRecycleCount = 0;
        Step();
    }

    uint32 targetIndex;
    if (!m_freeIndices.empty())
    {
        targetIndex = m_freeIndices.back();
        m_freeIndices.pop_back();
    }
    else if (!m_deadIndices.empty())
    {
        targetIndex = m_deadIndices.back();
        m_deadIndices.pop_back();
    }
    else
    {
        if (m_syncObjectCount >= m_syncObjectCapacity)
            Extend();
        targetIndex = (uint32_t)m_syncObjectCount++;
    }

    auto& obj = m_syncObjects[targetIndex];
    obj.generation++;
    obj.framesAgo = 0;
    obj.fenceSignaled = false;
    obj.semaphoreConsumed = false;
    obj.isActive = true;

    if (!m_fencePool.empty())
    {
        obj.fence = m_fencePool.back();
        m_fencePool.pop_back();
        vkResetFences(*m_device, 1, &obj.fence);
    }
    else
    {
        VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        vkCreateFence(*m_device, &fenceInfo, nullptr, &obj.fence);
    }

    if (!m_semaphorePool.empty())
    {
        obj.semaphore = m_semaphorePool.back();
        m_semaphorePool.pop_back();
    }
    else
    {
        VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        vkCreateSemaphore(*m_device, &semaphoreInfo, nullptr, &obj.semaphore);
    }

    return SyncTicket(obj.generation, targetIndex);
}

void VkSyncPool::Step()
{
    std::lock_guard lock(m_mutex);

    if (m_enableStats)
    {
        uint64_t frameCount = Graphics::GetFrameCount();
        if (frameCount != m_lastFrameCount)
        {
            m_finalizedStats = m_currentStats;
            m_finalizedStats.activeCount = 0;
            for (size_t i = 0; i < m_syncObjectCount; i++)
            {
                if (m_syncObjects[i].isActive)
                    m_finalizedStats.activeCount++;
            }
            m_finalizedStats.freeCount = (uint32_t)m_freeIndices.size();
            m_finalizedStats.deadCount = (uint32_t)m_deadIndices.size();
            m_finalizedStats.fencePoolSize = (uint32_t)m_fencePool.size();
            m_finalizedStats.semaphorePoolSize = (uint32_t)m_semaphorePool.size();
            m_finalizedStats.totalCount = (uint32_t)m_syncObjectCount;
            m_finalizedStats.capacity = (uint32_t)m_syncObjectCapacity;

            m_currentStats = {};
            m_lastFrameCount = frameCount;
        }
    }

    for (size_t i = 0; i < m_syncObjectCount; i++)
    {
        auto& obj = m_syncObjects[i];
        if (!obj.isActive)
            continue;

        if (obj.fence != VK_NULL_HANDLE)
        {
            if (vkGetFenceStatus(*m_device, obj.fence) == VK_SUCCESS)
            {
                obj.fenceSignaled = true;
                m_fencePool.push_back(obj.fence);
                obj.fence = VK_NULL_HANDLE;
            }
        }

        if (obj.fenceSignaled && !obj.semaphoreConsumed && obj.semaphore != VK_NULL_HANDLE)
        {
            if (obj.framesAgo++ > MAX_FRAMES_UNTIL_DECAY)
            {
                vkDestroySemaphore(*m_device, obj.semaphore, nullptr);
                obj.semaphore = VK_NULL_HANDLE;
                obj.semaphoreConsumed = true;
            }
        }

        if (obj.fenceSignaled && (obj.semaphoreConsumed || obj.semaphore == VK_NULL_HANDLE))
        {
            obj.isActive = false;
            obj.framesAgo = 0;
            m_freeIndices.push_back((uint32_t)i);
            
            if (m_enableStats)
                m_currentStats.retiredThisFrame++;
        }
    }
}

std::string VkSyncPool::GetStatsString() const
{
    std::lock_guard lock(m_mutex);
    char buffer[512];
    snprintf(buffer, sizeof(buffer),
        "Last Frame Stats:\n"
        "  Acquired: %u\n"
        "  Retired:  %u\n"
        "  Active:   %u\n"
        "  Free:     %u\n"
        "  Dead:     %u\n"
        "  Pools:    F:%u, S:%u\n"
        "  Total:    %u / %u (Cap)",
        m_finalizedStats.acquiredThisFrame,
        m_finalizedStats.retiredThisFrame,
        m_finalizedStats.activeCount,
        m_finalizedStats.freeCount,
        m_finalizedStats.deadCount,
        m_finalizedStats.fencePoolSize,
        m_finalizedStats.semaphorePoolSize,
        m_finalizedStats.totalCount,
        m_finalizedStats.capacity
    );
    return std::string(buffer);
}

void VkSyncPool::Extend()
{
    m_syncObjectCapacity += Graphics::GetMaxFramesInFlight() * EXTEND_SIZE;
    m_syncObjects = (SyncObject*)realloc(m_syncObjects, m_syncObjectCapacity * sizeof(SyncObject));
    HL_EXCEPTION(m_syncObjects == NULL, "Failed to extend VkSyncPool! Out of memory!");
}