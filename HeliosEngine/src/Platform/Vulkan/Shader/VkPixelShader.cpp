#include "VkPixelShader.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Resources/ResourceRegistry.h"
#include "Helios/Utils/Conversions.h"
#include "Helios/Utils/GetExecutablePath.h"

#include "VkShaderUtils.h"
#include "VkShaderCompiler.h"

#include <algorithm>
#include <filesystem>
#include <shaderc/shaderc.h>
#include <string>



namespace Helios::Internal
{    
    VkPixelShader::VkPixelShader(const Ref<VkGraphicsDevice>& device, const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        HL_EXCEPTION(shaderModules.size() <= 0, "At least 1 module must be present in PixelShader!");
        HL_EXCEPTION(Utils::HasDuplicateShaderModules(shaderModules), "You may not use the same ShaderModule multiple times!");
        
        m_key = ShaderKey(ShaderModule::Stage::Pixel, shaderModules);
        m_device = device;
        std::filesystem::path workingDir = ResourceRegistry::GetResourcePath();

        if (workingDir.empty()) workingDir = GetExeDir();

        auto combinedHash = Utils::CombineModulesHash(shaderModules);

        std::size_t i = 0;
        for (auto& module : shaderModules)
        {
            auto hash = module->GetHash();
            m_bindingMap[Hash128Key(hash.low64, hash.high64)] = i;
            i++;
        }


        
        std::string hashString = Utils::HashToString(combinedHash);

        std::filesystem::path spirv = workingDir / "Cache" / "Shaders" / "Compiled" / ( hashString + "_ps" + ".spv");
        std::filesystem::path nameMap = workingDir / "Cache" / "Shaders" / "Compiled" / ( hashString + "_ps" + ".map");

        if (std::filesystem::exists(spirv))
        {
            auto code = Utils::readFile(spirv);
            Utils::createShaderModule(device, code, m_pixelShaderModule);
            Utils::loadNameMap(nameMap, m_bindingLink);
            HL_EXCEPTION(!ReflectShaderBindings(code, m_bindingLayouts), "Error reflecting shader!");
            return;
        }
        
        auto mainModule = std::find_if(shaderModules.begin(), shaderModules.end(), [](Ref<ShaderModule> x) { return x->GetFamily() == ShaderModule::Family::Main; });
    
        HL_EXCEPTION(mainModule == shaderModules.end(), "Main module is missing from PixelShader!");

        std::vector<std::string> definitions;
        definitions.push_back("PIXEL");
        definitions.insert(definitions.end(), (*mainModule)->GetDefinitions().begin(), (*mainModule)->GetDefinitions().end());
        
        std::string shaderCode = ResolveShaderModule(*mainModule, shaderModules, m_bindingMap, m_bindingLink, definitions);
        
        bool save_hlsl = true;
        
        if (save_hlsl)
        {
            std::filesystem::path icp = workingDir / "Cache" / "Shaders" / "Intermediate" / ( "generated_" + hashString + "_ps" + ".hlsl");

            std::filesystem::create_directories(icp.parent_path());

            std::ofstream file(icp);
            HL_EXCEPTION(!file.is_open(), "Failed to open file for writing: " + conversions::from_u8string(icp.u8string()))

            file << shaderCode;
            file.close();
        }

        auto result = CompileHLSLtoSPIRV(shaderCode, Shader::Stage::Pixel);
        
        HL_EXCEPTION(!result.has_value(), "Error compiling shader:\n" + result.error());

        HL_EXCEPTION(!ReflectShaderBindings(result.value(), m_bindingLayouts), "Error reflecting shader!");

        bool save_spirv = true;
        
        if (save_spirv)
        {
            std::filesystem::create_directories(spirv.parent_path());

            std::ofstream file(spirv, std::ios::binary | std::ios::out);
            HL_EXCEPTION(!file.is_open(), "Failed to open file for writing: " + conversions::from_u8string(spirv.u8string()))

            file.write(reinterpret_cast<const char*>(result.value().data()), result.value().size());
            file.close();

            Utils::saveNameMap(nameMap, m_bindingLink);
        }

        Utils::createShaderModule(device, result.value(), m_pixelShaderModule);
    }
    VkPixelShader::VkPixelShader(const Ref<VkGraphicsDevice>& device, const std::filesystem::path &path)
    {
        m_device = device;
        m_key = ShaderKey(ShaderModule::Stage::Pixel, {});
        // m_bindingLayouts = bindingLayouts;
        Utils::createShaderModule(device, path, m_pixelShaderModule, shaderc_shader_kind::shaderc_fragment_shader);
    }
}