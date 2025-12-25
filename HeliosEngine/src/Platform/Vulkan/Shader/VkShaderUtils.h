#pragma once

#include "Helios/Core/Hash.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Resources/ShaderModule.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "pch.h"

#include "Helios/Core/Exceptions.h"
#include "Helios/Utils/Conversions.h"

#include "Platform/Vulkan/VkContext.h"

#include <shaderc/shaderc.hpp>

namespace Helios::Internal::Utils
{
    inline std::vector<char> hlslToSpirv(const std::string& filename, shaderc_shader_kind stage)
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
#include <shaderc/shaderc.hpp>

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

	inline std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
            HL_EXCEPTION(true, "Failed to open file: " + (conversions::from_u8string(std::filesystem::current_path().u8string()) + filename));
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

    inline std::vector<char> compileShader(const std::string& filename, shaderc_shader_kind stage)
    {
        if (filename.ends_with(".glsl")) return hlslToSpirv(filename, stage);
        else if (filename.ends_with(".hlsl")) return hlslToSpirv(filename, stage);
        else return readFile(filename);
    }

    inline void createShaderModule(const Ref<VkGraphicsDevice>& device, const std::vector<char>& binary, Vulkan::VkShaderModuleWrapper& shaderModule)
    {
        HL_EXCEPTION(binary.size() % 4 != 0, "SPIR-V binary size must be a multiple of 4");

        
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = binary.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(binary.data());
        
        auto result = Vulkan::VkShaderModuleWrapper::Create(*device, &createInfo, nullptr);
		HL_EXCEPTION(!result.has_value(), "Failed to create shader module");
        shaderModule = std::move(result.value());
        std::cout << "Loaded shader module: " << std::endl;
    }

    inline void createShaderModule(const Ref<VkGraphicsDevice>& device, const std::string& filename, Vulkan::VkShaderModuleWrapper& shaderModule, shaderc_shader_kind stage)
    {
        auto code = compileShader(filename, stage);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		auto result = Vulkan::VkShaderModuleWrapper::Create(*device, &createInfo, nullptr);
		HL_EXCEPTION(!result.has_value(), "Failed to create shader module");
        shaderModule = std::move(result.value());
        std::cout << "Loaded shader module: " << filename << std::endl;
    }

    inline XXH128_hash_t CombineModulesHash(const std::vector<Ref<ShaderModule>>& modules)
    {
        XXH3_state_t* state = XXH3_createState();
        XXH3_128bits_reset(state);

        // Extract hashes
        std::vector<XXH128_hash_t> hashes;
        hashes.reserve(modules.size());

        for (const auto& module : modules)
        {
            hashes.push_back(module->GetHash());
        }

        std::sort(hashes.begin(), hashes.end(), [](const XXH128_hash_t& a, const XXH128_hash_t& b) {
            if (a.high64 != b.high64) return a.high64 < b.high64;
            return a.low64 < b.low64;
        });

        for (const auto& h : hashes)
        {
            XXH3_128bits_update(state, &h, sizeof(h));
        }

        XXH128_hash_t combined = XXH3_128bits_digest(state);
        XXH3_freeState(state);

        return combined;
    }

    inline std::string HashToString(const XXH128_hash_t& hash)
    {
        char buffer[33] = {0};
        std::snprintf(buffer, sizeof(buffer), "%016llx%016llx",
              static_cast<unsigned long long>(hash.high64),
              static_cast<unsigned long long>(hash.low64));
        return std::string(buffer);
    }

    inline bool HasDuplicateShaderModules(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        std::unordered_set<Hash128Key, Hash128KeyHasher> seenHashes;
        
        for (const auto& module : shaderModules)
        {
            const auto& hash = module->GetHash();
            Hash128Key mHash(hash.low64, hash.high64);

            if (seenHashes.find(mHash) != seenHashes.end()) return true;
            seenHashes.insert(mHash);
        }
        return false;
    }

    inline void saveNameMap(const std::filesystem::path& path, const BindingLink& bindingLink)
    {
        // save as -> name -> BindingLink::Binding and again till end as binary

        std::ofstream file(path, std::ios::binary);
        HL_EXCEPTION(!file.is_open(), "Failed to open file for writing: " + conversions::from_u8string(path.u8string()))

        for (const auto& binding : bindingLink.GetBindings())
        {
            uint32_t nameLen = static_cast<uint32_t>(binding.first.size());
            file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            file.write(binding.first.data(), nameLen);

            file.write(reinterpret_cast<const char*>(&binding.second), sizeof(binding.second));
        }

        file.close();
    }

    inline void loadNameMap(const std::filesystem::path& path, BindingLink& bindingLink)
    {
        // load as -> name -> BindingLink::Binding and again till end
        
        std::ifstream file(path, std::ios::binary);
        HL_EXCEPTION(!file.is_open(), "Failed to open file for reading: " + conversions::from_u8string(path.u8string()))

        while (file.peek() != EOF)
        {
            uint32_t nameLen;
            file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            if (!file) break;

            std::string name(nameLen, '\0');
            file.read(&name[0], nameLen);
            if (!file) break;

            BindingLink::Binding binding;
            file.read(reinterpret_cast<char*>(&binding), sizeof(binding));
            if (!file) break;

            bindingLink.SetBinding(name, binding.type, binding.set, binding.slot);
        }

        file.close();
    }
    
}