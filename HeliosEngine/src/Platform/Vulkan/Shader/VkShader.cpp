#include "VkShader.h"

#include "Platform/Vulkan/VkContext.h"

#include <shaderc/shaderc.hpp>

namespace Helios
{
    std::vector<char> hlslToSpirv(const std::string& filename, shaderc_shader_kind stage)
    {


        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
        options.SetSourceLanguage(shaderc_source_language_hlsl);

        shaderc::Compiler compiler;
        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(buffer.data(), buffer.size(), stage, filename.c_str(), options);
        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
        {
#ifdef HELIOS_DEBUG
            std::string error_message = module.GetErrorMessage();
            std::cout << "SPIR-V compilation failed:" << std::endl << module.GetErrorMessage() << std::endl;
            HL_EXCEPTION(true, "Failed to compile HLSL to SPIR-V:\n" + error_message);
#else
            HL_EXCEPTION(true, "Failed to compile HLSL to SPIR-V");
#endif
        }
        return std::vector<char>(module.cbegin(), module.cend());
    }

	std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

    std::vector<char> compileShader(const std::string& filename, shaderc_shader_kind stage)
    {
        if (filename.ends_with(".glsl")) return hlslToSpirv(filename, stage);
        else if (filename.ends_with(".hlsl")) return hlslToSpirv(filename, stage);
        else return readFile(filename);
    }

    void createShaderModule(const std::string& filename, VkShaderModule* shaderModule, shaderc_shader_kind stage)
    {
        auto code = compileShader(filename, stage);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		HL_EXCEPTION(
			vkCreateShaderModule(
				VkContext::GetDevice(),
				&createInfo,
				nullptr,
				shaderModule
			) != VK_SUCCESS, "Failed to create shader module"
		);
        std::cout << "Loaded shader module: " << filename << std::endl;
    }

	VkShader::VkShader(const std::string& name, const std::string& vertex_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
	{
        createShaderModule(vertex_path, &m_vertexShader, shaderc_shader_kind::shaderc_vertex_shader);

        m_inputLayouts = inputLayouts;
        m_depthFunc = depthFunc;
        m_topology = topology;
	}
    VkShader::VkShader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
    {
		createShaderModule(vertex_path, &m_vertexShader, shaderc_shader_kind::shaderc_vertex_shader);
		createShaderModule(pixel_path, &m_pixelShader, shaderc_shader_kind::shaderc_fragment_shader);

        m_inputLayouts = inputLayouts;
        m_depthFunc = depthFunc;
        m_topology = topology;
    }
    VkShader::VkShader(const std::string& name, const std::string& vertex_path, const std::string& pixel_path, const std::string& geometry_path, InputLayouts inputLayouts, DepthFunc depthFunc, Topology topology)
    {
		createShaderModule(vertex_path, &m_vertexShader, shaderc_shader_kind::shaderc_vertex_shader);
		createShaderModule(pixel_path, &m_pixelShader, shaderc_shader_kind::shaderc_fragment_shader);
		createShaderModule(geometry_path, &m_geometryShader, shaderc_shader_kind::shaderc_geometry_shader);

        m_inputLayouts = inputLayouts;
		m_depthFunc = depthFunc;
		m_topology = topology;
    }
    void VkShader::Bind() const
    {
    }
    void VkShader::Unbind() const
    {
    }
}