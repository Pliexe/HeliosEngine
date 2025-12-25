#pragma once

#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/Sync/VkSyncableResource.h"

namespace Helios::Internal {
    class VkUniformBuffer final : public UnsafeUniformBuffer, public BaseVkSyncableResource
    {
    public:
        explicit VkUniformBuffer(const Ref<VkGraphicsDevice>& device, std::uint32_t size);
        ~VkUniformBuffer();
            
        VkBuffer GetBuffer() { return m_uniformBuffer.Get(); }      
        std::size_t GetSize() override { return m_Size; }

        // IVkSyncableResource
        VkPipelineStageFlags GetStage() const override { return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; }

        void SetData(const void *data) override;

		void* Lock(size_t size, size_t offset = 0) override;
		void Unlock() override;

        bool IsLocked() const override { return m_mappedData != nullptr; }
        size_t GetLockOffset() const override { return m_lockOffset; }
        size_t GetLockSize() const override { return m_lockSize; }
        
    private:
        friend class VkCommandList;
        friend class VkCommandListImmediate;
    
        Vulkan::VkBufferWrapper m_uniformBuffer;
        Vulkan::VkDeviceMemoryWrapper m_uniformBufferMemory;
        
        std::uint32_t m_Size;
        Ref<VkGraphicsDevice> m_device;

        Vulkan::VkBufferWrapper m_stagingBuffer;
        Vulkan::VkDeviceMemoryWrapper m_stagingBufferMemory;

        void* m_mappedData = nullptr;

        std::mutex m_mutex;

        size_t m_lockOffset;
        size_t m_lockSize;
    };
}