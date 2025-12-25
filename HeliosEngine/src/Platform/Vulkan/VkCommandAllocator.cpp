#include "VkCommandAllocator.h"

#include "Platform/Vulkan/CommandList/VkCommandListImmediate.h"
#include "Platform/Vulkan/VkContext.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "CommandList/VkCommandList.h"
#include <memory>

namespace Helios::Internal
{
    VkCommandAllocator::VkCommandAllocator(const Ref<GraphicsDevice>& device)
    {
        m_device = device;
        
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = VkContext::GetGraphicsQueueFamilyIndex();

        HL_EXCEPTION(vkCreateCommandPool(*std::static_pointer_cast<VkGraphicsDevice>(m_device), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS, "Failted to create command pool!");
    }

    VkCommandAllocator::~VkCommandAllocator()
    {
        vkDestroyCommandPool(*std::static_pointer_cast<VkGraphicsDevice>(m_device), m_commandPool, nullptr);    
    }
    Ref<NativeCommandList> VkCommandAllocator::CreateCommandList()
    {
        return CreateRef<VkCommandList>(std::static_pointer_cast<VkGraphicsDevice>(m_device), m_commandPool, false);
    }

    Ref<NativeCommandListImmediate> VkCommandAllocator::CreateCommandListImmediate()
    {
        return CreateRef<VkCommandListImmediate>(std::static_pointer_cast<VkGraphicsDevice>(m_device), m_commandPool, false);
    }
    void VkCommandAllocator::Reset()
    {
        vkResetCommandPool(*std::static_pointer_cast<VkGraphicsDevice>(m_device), m_commandPool, 0);
    }
}