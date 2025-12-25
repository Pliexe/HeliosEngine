
#pragma once

#include "Helios/Graphics/Sampler.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "VkIncludes.h"

namespace Helios::Internal
{
    class VkGraphicsDevice;
    class VkSampler final : public Sampler
    {
        public:
        VkSampler(const Ref<VkGraphicsDevice>& device, SamplerConfiguration config);
        ~VkSampler();
        
        ::VkSampler GetNative() { return m_sampler.Get(); }
        private:
        Vulkan::VkSamplerWrapper m_sampler;
        Ref<VkGraphicsDevice> m_device;
    };
}

#include "Platform/Vulkan/VkGraphicsDevice.h"