#include "VkVertexBuffer.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Resources/Buffer.h"
#include "Platform/Vulkan/VkContext.h"
#include "Platform/Vulkan/VkUtils.h"
#include "Platform/Vulkan/VkWrappers.h"
#include <mutex>

namespace Helios::Internal
{
	VkVertexBuffer::VkVertexBuffer(const Ref<VkGraphicsDevice>& device, const void* data, uint32_t size, BufferUsage usage)
        : BaseVkSyncableResource(device)
	{
		std::cout << "Vertex buffer size at creation vkvertexbuffer class : " << std::to_string(size) << std::endl;
		HL_EXCEPTION(size == 0, "Vertex buffer size cannot be 0");

		m_device = device;
		m_usage = usage;
		m_size = size;
		
		VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VkMemoryPropertyFlags memProperties = 0;

		switch (usage)
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
		bufferInfo.size = size;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		{
			auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
			HL_EXCEPTION(!result.has_value(), "Failed to create vertex buffer!");
			m_buffer = std::move(result.value());
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*m_device, m_buffer.Get(), &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = Utils::FindMemoryType(memRequirements.memoryTypeBits, memProperties, m_device->GetNativePhysicalDevice());

		{
			auto result = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
			HL_EXCEPTION(!result.has_value(), "Failed to create vertex buffer buffer!");
			m_bufferMemory = std::move(result.value());
		}

		vkBindBufferMemory(m_device->GetNativeDevice(), m_buffer.Get(), m_bufferMemory.Get(), 0);

		if (data && (memProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			void* mapped;
			vkMapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get(), 0, size, 0, &mapped);
			memcpy(mapped, data, size);
			vkUnmapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get());
		}
		else if (data)
		{
			UploadDataViaStagingBuffer(data, size);
		}

	}

	void VkVertexBuffer::UploadDataViaStagingBuffer(const void* data, uint32_t size)
	{
		Vulkan::VkBufferWrapper stagingBuffer;
		Vulkan::VkDeviceMemoryWrapper stagingBufferMemory;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		{
			auto result = Vulkan::VkBufferWrapper::Create(*m_device, &bufferInfo, nullptr);
			HL_EXCEPTION(!result.has_value(), "Failed to create staging buffer!");
			stagingBuffer = std::move(result.value());
		}

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

		{
			auto result = Vulkan::VkDeviceMemoryWrapper::Create(*m_device, &allocInfo, nullptr);
			HL_EXCEPTION(!result.has_value(), "Failed to allocate staging buffer memory!");
			stagingBufferMemory = std::move(result.value());
		}

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


	VkVertexBuffer::~VkVertexBuffer()
	{
		m_buffer.Reset();
		m_bufferMemory.Reset();
	}
	
	void VkVertexBuffer::SetData(const void* data, uint32_t size)
	{
		// TODO: REMOVE
		HL_EXCEPTION(m_usage == BufferUsage::Static, "Cannot set data on static buffer!");
		std::lock_guard lock(m_mutex);

		Wait(); // TODO: Single sync obj to use in future

		if (m_usage == BufferUsage::Dynamic || m_usage == BufferUsage::Stream)
		{
			void* mapped;
			vkMapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get(), 0, size, 0, &mapped);
			std::memcpy(mapped, data, size);
			vkUnmapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get());
		}
		else
		{
			UploadDataViaStagingBuffer(data, size);
		}
	}

	void* VkVertexBuffer::Lock(size_t size, size_t offset)
	{
		HL_EXCEPTION(m_usage == BufferUsage::Static, "Cannot lock static buffer!");
		
		HL_EXCEPTION(size > m_size, "Lock size is greater than buffer size!");
		HL_EXCEPTION(offset > m_size, "Lock offset is greater than buffer size!");
		HL_EXCEPTION(offset + size > m_size, "Lock offset + size is greater than buffer size!");
	
		m_lock.lock();
		m_threadId = std::this_thread::get_id();

		Wait();
		
		bool useStagingBuffer = InFlight();

		void* mapped;
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

			HL_EXCEPTION(vkMapMemory(*m_device, m_stagingBufferMemory.Get(), offset, size, 0, &mapped) != VK_SUCCESS || mapped == nullptr, "Failed to map staging buffer memory!");

			m_lockOffset = offset;
			m_lockSize = size;

			return mapped;
		}
		else
		{
			HL_EXCEPTION(vkMapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get(), offset, size, 0, &mapped) != VK_SUCCESS || mapped == nullptr, "Failed to map memory!");

			m_lockOffset = offset;
			m_lockSize = size;
		}

		return mapped;
	}

	void VkVertexBuffer::Unlock()
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
			
			vkCmdCopyBuffer(cmd, m_stagingBuffer.Get(), m_buffer.Get(), 1, &copyRegion);
			
			m_device->LegacyEndSingleTimeCommands(cmd);

			m_stagingBuffer.Reset();
			m_stagingBufferMemory.Reset();
		}
		else
		{
			vkUnmapMemory(m_device->GetNativeDevice(), m_bufferMemory.Get());
		}

		m_mappedData = nullptr;
		m_lockOffset = 0;
		m_lockSize = 0;
		m_lock.unlock();
	}
}
