#pragma once

#include  "../VkIncludes.h"

#include "Helios/Resources/Shader.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkWrappers.h"

namespace Helios::Internal
{
    class VkPixelShader : public PixelShader
    {
    public:
        VkPixelShader(const Ref<VkGraphicsDevice>& device, const std::vector<Ref<ShaderModule>>& shaderModules);
        VkPixelShader(const Ref<VkGraphicsDevice>& device, const std::filesystem::path &path);
    
        ~VkPixelShader() = default;

        const VkShaderModule GetHandle() const { return m_pixelShaderModule.Get(); }
    private:
        Vulkan::VkShaderModuleWrapper m_pixelShaderModule;
        Ref<VkGraphicsDevice> m_device;
    };
}