#pragma once

#include "pch.h"
#include "VkIncludes.h"
#include "Helios/Core/Exceptions.h"

namespace Helios::Internal::Utils
{
    inline uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && 
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        HL_EXCEPTION(true, "Failed to find suitable memory type!");
    }
}