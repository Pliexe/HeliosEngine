#pragma once

#include "Helios/Core/Exceptions.h"
#include "Platform/Vulkan/Sync/VkSyncPool.h"

namespace Helios::Internal
{
    class IVkSyncableResource
    {
    public:
        IVkSyncableResource() = default;
        virtual ~IVkSyncableResource() = default;

        virtual void AddRead(SyncTicket ticket) = 0;
        virtual void SetWrite(SyncTicket ticket) = 0;

        virtual bool InFlight() const = 0;

        virtual void Wait() const = 0;
        virtual void Reset() const = 0;

        virtual bool HasRead() const = 0;
        virtual bool HasWrite() const = 0;

        virtual void Clean() = 0;

        virtual SyncTicket GetWrite() const = 0;
        virtual std::vector<SyncTicket> GetRead() const = 0;

        virtual VkPipelineStageFlags GetStage() const = 0;
        
        virtual std::thread::id GetThreadID() const = 0;
        virtual std::mutex& GetLock() = 0;
        
    };

    class VkGraphicsDevice;

    class BaseVkSyncableResource : public IVkSyncableResource
    {
    public:
        BaseVkSyncableResource(Ref<VkGraphicsDevice> device);
        virtual ~BaseVkSyncableResource() = default;

        void AddRead(SyncTicket ticket) override;
        void SetWrite(SyncTicket ticket) override;

        bool InFlight() const override;

        void Wait() const override;
        void Reset() const override;

        bool HasRead() const override;
        bool HasWrite() const override;

        void Clean() override;

        SyncTicket GetWrite() const override;
        std::vector<SyncTicket> GetRead() const override;

        std::thread::id GetThreadID() const override { return m_threadId; }
        std::mutex& GetLock() override { return m_lock; }

    protected:
        Ref<VkGraphicsDevice> m_device;
        SyncTicket m_writeTicket;
        std::vector<SyncTicket> m_readTickets;
        bool m_hasWrite = false;

        std::mutex m_lock;
        std::thread::id m_threadId;
    };

    struct VkResourceSyncList
    {
        std::vector<Ref<IVkSyncableResource>> readResources;
        std::vector<Ref<IVkSyncableResource>> writeResources;

        void AddRead(Ref<IVkSyncableResource> resource) { readResources.push_back(resource); HL_EXCEPTION(readResources.size() > 20, "Too many read resources"); }
        void AddWrite(Ref<IVkSyncableResource> resource) { writeResources.push_back(resource); HL_EXCEPTION(writeResources.size() > 20, "Too many write resources"); }

        void Normalize()
        {
            std::sort(readResources.begin(), readResources.end());
            readResources.erase(std::unique(readResources.begin(), readResources.end()), readResources.end());
            
            std::sort(writeResources.begin(), writeResources.end());
            writeResources.erase(std::unique(writeResources.begin(), writeResources.end()), writeResources.end());
        }

        void Clear()
        {
            readResources.clear();
            writeResources.clear();
        }
    };
}