#pragma once

#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/Sync/VkSyncableResource.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"

namespace Helios::Internal
{
    class HELIOS_API VkIndexBuffer final : public IndexBuffer, public BaseVkSyncableResource
    {
    public:
        VkIndexBuffer(const Ref<VkGraphicsDevice>& device, uint32_t* indices, uint32_t count, BufferUsage usage);

        ~VkIndexBuffer();

        uint32 GetCount() const override;
        std::string ToString() const override;

        void SetData(const uint32_t* data, uint32_t count) override;


        ::VkBuffer GetBuffer() { return m_buffer.Get(); }

        // IVkSyncableResource
        VkPipelineStageFlags GetStage() const override { return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; }

    private:

        void UploadDataViaStagingBuffer(const void* data, uint32_t size);
        VkMemoryPropertyFlags CreateBuffer();

    
        Vulkan::VkBufferWrapper m_buffer;
        Vulkan::VkDeviceMemoryWrapper m_bufferMemory;
        BufferUsage m_usage;

        uint32_t m_size;
        uint32_t m_count;
        uint32_t m_binding_slot;
        uint32_t* m_data;

        std::mutex m_mutex;



        Ref<VkGraphicsDevice> m_device;
    };
}
