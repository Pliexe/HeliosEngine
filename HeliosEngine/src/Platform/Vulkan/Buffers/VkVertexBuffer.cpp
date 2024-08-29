#include "VkVertexBuffer.h"

#include <vulkan/vulkan.h>

namespace Helios
{
	VkVertexBuffer::VkVertexBuffer(const void* data, uint32_t size, BufferUsage usage)
	{
		//vkCmdBindVertexBuffers()
	}

	VkVertexBuffer::VkVertexBuffer(uint32_t size, BufferUsage usage)
	{
		
	}
	void VkVertexBuffer::Bind(uint32_t slot) const
	{
	}
	void VkVertexBuffer::Unbind(uint32_t slot) const
	{
	}
	void VkVertexBuffer::SetData(const void* data, uint32_t size)
	{
	}
	void VkVertexBuffer::SetInputLayout(const InputLayout& layout)
	{
	}
	const InputLayout& VkVertexBuffer::GetInputLayout() const
	{
		// TODO: insert return statement here
	}
}