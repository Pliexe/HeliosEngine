#include "VkSampler.h"

#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "VkContext.h"

namespace Helios::Internal
{
    VkFilter GetTextureFilter(TextureFilter filter)
    {
        switch (filter)
        {
            case TextureFilter::Linear: return VK_FILTER_LINEAR;
            case TextureFilter::Nearest: return VK_FILTER_NEAREST;
        }
        HL_EXCEPTION(true, "Uknown TextureFilter!");
    }

    VkSamplerAddressMode GetWrap(TextureWrap wrap)
    {
        switch (wrap)
        {
            case TextureWrap::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case TextureWrap::Border: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case TextureWrap::Clamp: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case TextureWrap::Mirror: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT; 
        }
    }
    
    VkSampler::VkSampler(const Ref<VkGraphicsDevice>& device, SamplerConfiguration config)
    {
        m_device = device;
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = GetTextureFilter(config.magFilter);
        samplerInfo.minFilter = GetTextureFilter(config.minFilter);
        samplerInfo.addressModeU = GetWrap(config.wrapU);
        samplerInfo.addressModeV = GetWrap(config.wrapV);
        samplerInfo.addressModeW = GetWrap(config.wrapW);

        samplerInfo.anisotropyEnable = VK_FALSE;
        // samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = m_device->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        auto result = Vulkan::VkSamplerWrapper::Create(*m_device, &samplerInfo, nullptr);
        HL_EXCEPTION(!result.has_value(), "Failed to create VkSampler!");
        m_sampler = std::move(result.value());        
    }
    VkSampler::~VkSampler()
    {
        m_sampler.Reset();
    }
}