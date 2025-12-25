#include "Shader.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{

    void VertexShader::Validate(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        size_t mains = 0;
        for (auto& shaderModule : shaderModules)
        {
            if (none(shaderModule->GetStage() & ShaderModule::Stage::Vertex))
            {
                HL_EXCEPTION(true, "ShaderModule stage given to VertexShader is not of Vertex stage!");
            }
            if (shaderModule->GetFamily() == ShaderModule::Family::Main)
            {
                HL_EXCEPTION(mains > 0, "Only 1 Main Shader may be present in the list of ShaderModules!");
                mains++;
            }
        }
    }
    
    Ref<VertexShader> VertexShader::Create(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        return Graphics::GetMainDevice()->CreateVertexShader(shaderModules);
    }

    Ref<VertexShader> VertexShader::Create(const std::filesystem::path &path, InputLayouts inputLayouts)
    {
        return Graphics::GetMainDevice()->CreateVertexShader(path, inputLayouts);
    }

    void PixelShader::Validate(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        size_t mains = 0;
        for (auto& shaderModule : shaderModules)
        {
            if (none(shaderModule->GetStage() & ShaderModule::Stage::Pixel))
            {
                HL_EXCEPTION(true, "ShaderModule stage given to PixelShader is not of Pixel stage!");
            }
            if (shaderModule->GetFamily() == ShaderModule::Family::Main)
            {
                HL_EXCEPTION(mains > 0, "Only 1 Main Shader may be present in the list of ShaderModules!");
                mains++;
            }
        }
    }

    Ref<PixelShader> PixelShader::Create(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        return Graphics::GetMainDevice()->CreatePixelShader(shaderModules);
    }

    Ref<PixelShader> PixelShader::Create(const std::filesystem::path &path)
    {        
        return Graphics::GetMainDevice()->CreatePixelShader(path);
    }

    void GeometryShader::Validate(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        size_t mains = 0;
        for (auto& shaderModule : shaderModules)
        {
            if (none(shaderModule->GetStage() & ShaderModule::Stage::Geometry))
            {
                HL_EXCEPTION(true, "ShaderModule stage given to GeometryShader is not of Geometry stage!");
            }
            if (shaderModule->GetFamily() == ShaderModule::Family::Main)
            {
                HL_EXCEPTION(mains > 0, "Only 1 Main Shader may be present in the list of ShaderModules!");
                mains++;
            }
        }
    }

    Ref<GeometryShader> GeometryShader::Create(const std::vector<Ref<ShaderModule>>& shaderModules)
    {
        return Graphics::GetMainDevice()->CreateGeometryShader(shaderModules);
    }

    Ref<GeometryShader> GeometryShader::Create(const std::filesystem::path &path)
    {        
        return Graphics::GetMainDevice()->CreateGeometryShader(path);
    }

    
} // namespace Helios