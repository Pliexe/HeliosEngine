#include "VkUniformBuffer.h"
#include "Helios/Core/Exceptions.h"

#include "Platform/Vulkan/VkUtils.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include <vulkan/vulkan_core.h>


namespace Helios::Internal
{

     VkUniformBuffer::VkUniformBuffer(const Ref<VkGraphicsDevice> &device, std::uint32_t size)
    : BaseVkSyncableResource(device), m_device(device), m_Size(size)
    {
        HL_EXCEPTION(size == 0, "UniformBuffer size cannot be 0");
        if (size == MAX_BUFFER_SIZE) {
            size = m_Size = m_device->GetPhysicalDeviceProperties().limits.maxUniformBufferRange;
        }

        HL_EXCEPTION(size > m_device->GetPhysicalDeviceProperties().limits.maxUniformBufferRange, std::format("Cannot allocate UniformBuffer of size {}b, because the maximum size for this device is {}b.", size, size > m_device->GetPhysicalDeviceProperties().limits.maxUniformBufferRange))

        VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        // TODO: Make it configurable

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


        {
            auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
            HL_EXCEPTION(!result.has_value(), "Failed to create uniform buffer!");
            m_uniformBuffer = std::move(result.value());
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*m_device, m_uniformBuffer.Get(), &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Utils::FindMemoryType(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_device->GetNativePhysicalDevice()
        );

        {
            auto result = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
            HL_EXCEPTION(!result.has_value(), "Failed to allocate uniform buffer memory!");
            m_uniformBufferMemory = std::move(result.value());
        }

        vkBindBufferMemory(*m_device, m_uniformBuffer.Get(), m_uniformBufferMemory.Get(), 0);

    }

    VkUniformBuffer::~VkUniformBuffer()
    {
        // if (m_device->GetState() == GraphicsDevice::State::Initialized)
        // {
        Wait();
        // } else {
        //     std::cerr << "Unable to destroy UniformBuffer from GPU memory due to Device not initialized instead is in: " << m_device->GetStateName() << std::endl;
        // }
    }

    void VkUniformBuffer::SetData(const void *data)
    {
        Wait();

        void* mapped = nullptr;
        if (vkMapMemory(*m_device, m_uniformBufferMemory.Get(), 0, m_Size, 0, &mapped) != VK_SUCCESS || !mapped)
            return;

        std::memcpy(mapped, data, m_Size);

        vkUnmapMemory(*m_device, m_uniformBufferMemory.Get());
    }

    void* VkUniformBuffer::Lock(size_t size, size_t offset)
    {
        HL_EXCEPTION(size > m_Size, "Locking UniformBuffer out of bounds! {} > {}", size, m_Size);
        HL_EXCEPTION(offset > m_Size, "Locking UniformBuffer out of bounds! {} > {}", offset, m_Size);
        HL_EXCEPTION(size + offset > m_Size, "Locking UniformBuffer out of bounds! {} + {} > {}", size, offset, m_Size);

        HL_EXCEPTION(m_mappedData != nullptr || m_stagingBufferMemory.Get() != nullptr, "UniformBuffer already mapped!");

        m_lock.lock();
        m_threadId = std::this_thread::get_id();

        Wait();

        bool useStagingBuffer = InFlight();

        if (useStagingBuffer)
        {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            {
                auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
                HL_EXCEPTION(!result.has_value(), "Failed to create staging buffer!");
                m_stagingBuffer = std::move(result.value());
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(*m_device, m_stagingBuffer.Get(), &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = Utils::FindMemoryType(
                memRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                m_device->GetNativePhysicalDevice()
            );

            {
                auto result = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
                HL_EXCEPTION(!result.has_value(), "Failed to allocate staging buffer memory!");
                m_stagingBufferMemory = std::move(result.value());
            }

            vkBindBufferMemory(*m_device, m_stagingBuffer.Get(), m_stagingBufferMemory.Get(), 0);

            void* mapped = nullptr;
            if (vkMapMemory(*m_device, m_stagingBufferMemory.Get(), offset, size, 0, &mapped) != VK_SUCCESS || !mapped)
                return nullptr;

            m_mappedData = mapped;
            m_lockOffset = offset;
            m_lockSize = size;

            return mapped;
        }
        else
        {
            void* mapped = nullptr;
            if (vkMapMemory(*m_device, m_uniformBufferMemory.Get(), offset, size, 0, &mapped) != VK_SUCCESS || !mapped)
                return nullptr;

            m_mappedData = mapped;
            m_lockOffset = offset;
            m_lockSize = size;

            return mapped;   
        }
        
    }

    void VkUniformBuffer::Unlock()
    {
        
        if (m_stagingBuffer.Get() != VK_NULL_HANDLE)
        {
            vkUnmapMemory(*m_device, m_stagingBufferMemory.Get());

            Wait();

            auto cmd = m_device->LegacyBeginSingleTimeCommands();
            
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = m_lockOffset;
            copyRegion.size = m_lockSize;

            vkCmdCopyBuffer(cmd, m_stagingBuffer.Get(), m_uniformBuffer.Get(), 1, &copyRegion);

            m_device->LegacyEndSingleTimeCommands(cmd);

            m_stagingBuffer.Reset();
            m_stagingBufferMemory.Reset();
        }
        else
        {
            vkUnmapMemory(*m_device, m_uniformBufferMemory.Get());
        }

        m_mappedData = nullptr;
        m_lockOffset = 0;
        m_lockSize = 0;
        m_lock.unlock();
    }
}
