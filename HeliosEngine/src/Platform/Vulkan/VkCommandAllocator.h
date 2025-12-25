#pragma once

#include "Helios/Graphics/GraphicsDevice.h"
#include "pch.h"
#include "Helios/Core/Base.h"

#include "Helios/Graphics/CommandAllocator.h"

#include "VkIncludes.h"

namespace Helios::Internal
{
    class HELIOS_API VkCommandAllocator final : public CommandAllocator
    {
    public:
        VkCommandAllocator(const Ref<GraphicsDevice>& device);
        ~VkCommandAllocator();

        Ref<NativeCommandListImmediate> CreateCommandListImmediate() override;
        Ref<NativeCommandList> CreateCommandList() override;

        // Ref<NativeCommandListImmediate> CreateTransientCommandListImmediate() override;
        // Ref<NativeCommandList> CreateTransientCommandList() override;

        void Reset() override;
        
    private:
        friend class VkContext;
        friend class VkGraphicsDevice;

        VkCommandPool m_commandPool;
        Ref<GraphicsDevice> m_device;
    };
}