#pragma once

#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/Sync/VkSyncableResource.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"

namespace Helios::Internal
{
    class VkVertexBuffer final : public UnsafeVertexBuffer, public BaseVkSyncableResource
    {
    public:
    public:
        VkVertexBuffer(const Ref<VkGraphicsDevice>& device, const void* data, uint32_t size, BufferUsage usage);

        ~VkVertexBuffer();

        void SetData(const void* data, uint32_t size) override;
        uint32_t GetSize() const override { return m_size; }

        void* Lock(size_t size, size_t offset = 0) override;
		void Unlock() override;

        bool IsLocked() const override { return m_mappedData != nullptr; }
        size_t GetLockOffset() const override { return m_lockOffset; }
        size_t GetLockSize() const override { return m_lockSize; }

        VkVertexInputBindingDescription GetBindingDescriptor(uint32_t stride)
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = stride;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        ::VkBuffer GetBuffer() { return m_buffer.Get(); }

        // IVkSyncableResource
        VkPipelineStageFlags GetStage() const override { return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; }

    private:
        friend class VkCommandList;
        friend class VkCommandListImmediate;

        void UploadDataViaStagingBuffer(const void* data, uint32_t size);
     
        Vulkan::VkBufferWrapper m_buffer;
        Vulkan::VkDeviceMemoryWrapper m_bufferMemory;
        BufferUsage m_usage;
        uint32_t m_size;

        Vulkan::VkBufferWrapper m_stagingBuffer;
        Vulkan::VkDeviceMemoryWrapper m_stagingBufferMemory;
        
        void* m_mappedData = nullptr;

        size_t m_lockOffset;
        size_t m_lockSize;

        std::mutex m_mutex;

        Ref<VkGraphicsDevice> m_device;
    };
}
