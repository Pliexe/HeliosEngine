#pragma once

#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkIncludes.h"

#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/Shader.h"
#include "Platform/Vulkan/VkWrappers.h"

namespace Helios::Internal
{
    class VkVertexShader : public VertexShader
    {
    public:
        VkVertexShader(const Ref<VkGraphicsDevice>& device, const std::vector<Ref<ShaderModule>>& shaderModules);
        VkVertexShader(const Ref<VkGraphicsDevice>& device, const std::filesystem::path &path, InputLayouts inputLayouts);
        ~VkVertexShader() = default;
    
		const InputLayouts& GetInputLayouts() const override { return m_inputLayouts; }

        const VkShaderModule GetHandle() const { return m_vertexShaderModule.Get(); }
    private:
        InputLayouts m_inputLayouts;

        Vulkan::VkShaderModuleWrapper m_vertexShaderModule;
        Ref<VkGraphicsDevice> m_device;
    };
}