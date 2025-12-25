#include "Platform/Vulkan/Sync/VkSyncableResource.h"
#include "Helios/Core/Asserts.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/Sync/VkSyncPool.h"

namespace Helios::Internal
{
    BaseVkSyncableResource::BaseVkSyncableResource(Ref<VkGraphicsDevice> device)
        : m_device(device)
    {
    }

    void BaseVkSyncableResource::AddRead(SyncTicket ticket)
    {
        HL_ASSERT(m_readTickets.size() < 50, "Too many read tickets!");

        m_readTickets.push_back(ticket);
    }

    void BaseVkSyncableResource::SetWrite(SyncTicket ticket)
    {
        m_writeTicket = ticket;
        m_hasWrite = true;
        m_readTickets.clear();
    }

    void BaseVkSyncableResource::Wait() const
    {
        if (m_hasWrite) m_device->GetSyncPool().Wait(m_writeTicket);
        if (!m_readTickets.empty()) m_device->GetSyncPool().Wait(m_readTickets);
    }

    void BaseVkSyncableResource::Reset() const
    {
        // No-op for now
    }

    bool BaseVkSyncableResource::HasRead() const
    {
        return !m_readTickets.empty();
    }

    bool BaseVkSyncableResource::HasWrite() const
    {
        return m_hasWrite;
    }

    void BaseVkSyncableResource::Clean()
    {
        // Checks if ticket is valid

        if (m_hasWrite && !m_device->GetSyncPool().IsValid(m_writeTicket))
        {
            m_hasWrite = false;
            m_writeTicket = SyncTicket();
        }
        if (!m_readTickets.empty())
        {
            for (auto it = m_readTickets.begin(); it != m_readTickets.end();)
            {
                if (!m_device->GetSyncPool().IsValid(*it))
                    it = m_readTickets.erase(it);
                else
                    it++;
            }
        }
    }

    SyncTicket BaseVkSyncableResource::GetWrite() const
    {
        return m_writeTicket;
    }

    std::vector<SyncTicket> BaseVkSyncableResource::GetRead() const
    {
        return m_readTickets;
    }

    bool BaseVkSyncableResource::InFlight() const
    {
        if (m_hasWrite && m_device->GetSyncPool().IsInFlight(m_writeTicket))
            return true;
        if (!m_readTickets.empty() && m_device->GetSyncPool().IsInFlight(m_readTickets))
            return true;
        return false;
    }
}
