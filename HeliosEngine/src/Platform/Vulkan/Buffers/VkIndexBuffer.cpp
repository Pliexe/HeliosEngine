#include "VkIndexBuffer.h"
#include "Helios/Core/Exceptions.h"
#include "Platform/Vulkan/VkUtils.h"

namespace Helios::Internal
{
    VkIndexBuffer::VkIndexBuffer(const Ref<VkGraphicsDevice>& device, uint32_t* indices, uint32_t count, BufferUsage usage)
        : BaseVkSyncableResource(device)
    {
        HL_EXCEPTION(indices == nullptr, "Index buffer cannot be null");
        HL_EXCEPTION(count == 0, "Index buffer count cannot be 0");

        m_device = device;
        m_usage = usage;
        m_count = count;
        m_size = count * sizeof(uint32_t);

        m_data = new uint32_t[count];
        std::memcpy(m_data, indices, m_size);
        
        auto memProperties = CreateBuffer();
        
        if (indices && (memProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
        {
            void* mapped;
            vkMapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get(), 0, m_size, 0, &mapped);
            std::memcpy(mapped, indices, m_size);
            vkUnmapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get());
        }
        else if (indices)
        {
            UploadDataViaStagingBuffer(indices, m_size);
        }
    }

    VkMemoryPropertyFlags VkIndexBuffer::CreateBuffer()
    {
        VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        VkMemoryPropertyFlags memProperties = 0;

        switch (m_usage)
        {
            case BufferUsage::Static:
                usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                break;
            case BufferUsage::Dynamic:
            case BufferUsage::Stream:
                memProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;
            case BufferUsage::Staging:
                memProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                break;
        }

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = m_size;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create index buffer!");
        m_buffer = std::move(result.value());

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*m_device, m_buffer.Get(), &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Utils::FindMemoryType(
            memRequirements.memoryTypeBits,
            memProperties,
            m_device->GetNativePhysicalDevice()
        );

        auto memResult = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
        HL_EXCEPTION(!memResult.has_value(), "Failed to allocate index buffer memory!");
        m_bufferMemory = std::move(memResult.value());

        vkBindBufferMemory(m_device->GetNativeDevice(), m_buffer.Get(), m_bufferMemory.Get(), 0);

        return memProperties;
    }

    void VkIndexBuffer::UploadDataViaStagingBuffer(const void* data, uint32_t size)
    {
        Vulkan::VkBufferWrapper stagingBuffer;
        Vulkan::VkDeviceMemoryWrapper stagingBufferMemory;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create staging buffer!");
        stagingBuffer = std::move(result.value());

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*m_device, stagingBuffer.Get(), &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Utils::FindMemoryType(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_device->GetNativePhysicalDevice()
        );

        auto memResult = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
        HL_EXCEPTION(!memResult.has_value(), "Failed to allocate staging buffer memory!");
        stagingBufferMemory = std::move(memResult.value());

        vkBindBufferMemory(*m_device, stagingBuffer.Get(), stagingBufferMemory.Get(), 0);

        void* mapped;
        vkMapMemory(*m_device, stagingBufferMemory.Get(), 0, size, 0, &mapped);
        std::memcpy(mapped, data, size);
        vkUnmapMemory(*m_device, stagingBufferMemory.Get());

        VkCommandBuffer commandBuffer = m_device->LegacyBeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, stagingBuffer.Get(), m_buffer.Get(), 1, &copyRegion);

        m_device->LegacyEndSingleTimeCommands(commandBuffer);
    }

    VkIndexBuffer::~VkIndexBuffer()
    {
        delete[] m_data;
        m_buffer.Reset();
        m_bufferMemory.Reset();
    }

    uint32 VkIndexBuffer::GetCount() const
    {
        return m_count;
    }

    std::string VkIndexBuffer::ToString() const
    {
        if (m_count == 0) return "";

        std::string str = std::to_string(m_data[0]);
        for (uint32_t i = 1; i < m_count; i++)
        {
            str += (i % 3 == 0 ? "\n" : ", ") + std::to_string(m_data[i]);
        }
        return str;
    }

    void VkIndexBuffer::SetData(const uint32_t* data, uint32_t count)
    {
        HL_EXCEPTION(m_usage == BufferUsage::Static, "Cannot set data on static buffer!");
        std::lock_guard lock(m_mutex);

        Wait();

        if (count != m_count)
        {
            delete[] m_data;
            m_data = new uint32_t[count];
            m_count = count;
            m_size = count * sizeof(uint32_t);
            
            m_buffer.Reset();
            m_bufferMemory.Reset();

            CreateBuffer();
        }

        std::memcpy(m_data, data, count * sizeof(uint32_t));

        if (m_usage == BufferUsage::Dynamic || m_usage == BufferUsage::Stream)
        {
            void* mapped;
            vkMapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get(), 0, m_size, 0, &mapped);
            std::memcpy(mapped, data, m_size);
            vkUnmapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get());
        }
        else
        {
            UploadDataViaStagingBuffer(data, m_size);
        }
    }
}
