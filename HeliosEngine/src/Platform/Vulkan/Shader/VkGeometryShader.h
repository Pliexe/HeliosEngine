#pragma once

#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkIncludes.h"

#include "Helios/Resources/Shader.h"
#include "Platform/Vulkan/VkWrappers.h"

namespace Helios::Internal
{
    class VkGeometryShader : public GeometryShader
    {
    public:
        VkGeometryShader(const Ref<VkGraphicsDevice>& device, const std::vector<Ref<ShaderModule>>& shaderModules);
        VkGeometryShader(const Ref<VkGraphicsDevice>& device, const std::filesystem::path &path);

        ~VkGeometryShader() = default;

        const VkShaderModule GetHandle() const { return m_geometryShaderModule.Get(); }
    private:
        Vulkan::VkShaderModuleWrapper m_geometryShaderModule;
        Ref<VkGraphicsDevice> m_device;
    };
}