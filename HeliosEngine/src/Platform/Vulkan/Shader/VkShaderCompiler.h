#pragma once

#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Utils/Conversions.h"
#include "pch.h"

#include "Helios/Resources/Shader.h"
#include "Helios/Utils/Expected.h"

#include <dxc/dxcapi.h>
#include <regex>

#include <spirv_reflect.h>
#include <string_view>
#include <unordered_map>

namespace Helios::Internal
{
    inline std::string GetDxcErrors(IDxcBlobEncoding* errors) {
        if (!errors) return "No errors.";
        size_t size = errors->GetBufferSize();
        if (size == 0) return "No errors.";
        const char* msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
        if (msg[size - 1] == '\0') size -= 1;
        return std::string(msg, size);
    }

    inline Helios::Utils::Expected<std::vector<char>, std::string> CompileHLSLtoSPIRV(const std::string& source, Shader::Stage stage) {
        IDxcUtils* utils = nullptr;
        IDxcCompiler3* compiler3 = nullptr;
        IDxcIncludeHandler* includeHandler = nullptr;
        IDxcResult* result = nullptr;

        HRESULT hr = S_OK;

        hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
        if (FAILED(hr)) { return std::string("Failed to create DXC Utils"); }

        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler3));
        if (FAILED(hr)) { utils->Release(); return std::string("Failed to create DXC Compiler3"); }

        hr = utils->CreateDefaultIncludeHandler(&includeHandler);
        if (FAILED(hr)) { compiler3->Release(); utils->Release(); return std::string("Failed to create include handler"); }

        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = source.data();
        sourceBuffer.Size = source.size();
        sourceBuffer.Encoding = DXC_CP_UTF8;

        std::wstring targetProfile;
        switch (stage) {
            case Shader::Stage::Vertex: targetProfile = L"vs_6_0"; break;
            case Shader::Stage::Pixel:  targetProfile = L"ps_6_0"; break;
            case Shader::Stage::Geometry:  targetProfile = L"gs_6_0"; break;
            case Shader::Stage::Compute: targetProfile = L"cs_6_0"; break;
        }

        std::wstring entryFun;

        switch (stage) {
            case Shader::Stage::Vertex: entryFun = L"MainVS"; break;
            case Shader::Stage::Pixel:  entryFun = L"MainPS"; break;
            case Shader::Stage::Geometry:  entryFun = L"MainGS"; break;
            case Shader::Stage::Compute: entryFun = L"MainCS"; break;
        }

        std::vector<LPCWSTR> arguments = {
            L"-E", entryFun.c_str(),
            L"-T", targetProfile.c_str(),
            L"-spirv",
            #ifdef HELIOS_DEBUG
            L"-fspv-debug=vulkan-with-source",
            L"-Zi",
            L"Qembed_debug",
            L"fspv-target-env=vulkan1.0",
            #else
            L"-O3", // Optimization level
            #endif
        };

        hr = compiler3->Compile(
            &sourceBuffer,
            arguments.data(),
            static_cast<UINT32>(arguments.size()),
            includeHandler,
            IID_PPV_ARGS(&result)
        );

        if (FAILED(hr)) {
            // IDxcBlobEncoding* errors;
            // hr = result->GetErrorBuffer(&errors);
            // if (SUCCEEDED(hr) && errors) {
            //     PrintDxcErrors(errors);
            //     errors->Release();
            // }
            
            includeHandler->Release();
            compiler3->Release();
            utils->Release();
            std::ostringstream oss;
            oss << "Compile call failed with HRESULT 0x" << std::hex << hr;
            return oss.str();
        }

        HRESULT status;
        hr = result->GetStatus(&status);
        if (FAILED(hr) || FAILED(status)) {
            IDxcBlobEncoding* errors = nullptr;

            std::string err = "No Error.";

            hr = result->GetErrorBuffer(&errors);
            if (SUCCEEDED(hr) && errors) {
                err = GetDxcErrors(errors);
                errors->Release();
            }
            
            result->Release();
            includeHandler->Release();
            compiler3->Release();
            utils->Release();
            return err;
        }

        IDxcBlob* compiledBlob = nullptr;
        hr = result->GetResult(&compiledBlob);
        if (FAILED(hr)) {
            result->Release();
            includeHandler->Release();
            compiler3->Release();
            utils->Release();
            return std::string("Failed to get compiled blob");
        }

        std::vector<char> spirvOutput;

        // Copy SPIR-V bytecode into output vector
        spirvOutput.resize(compiledBlob->GetBufferSize());
        memcpy(spirvOutput.data(), compiledBlob->GetBufferPointer(), compiledBlob->GetBufferSize());

        // Release interfaces
        compiledBlob->Release();
        result->Release();
        includeHandler->Release();
        compiler3->Release();
        utils->Release();

        return spirvOutput;
    }



    inline std::string ConstructImport(const Ref<ShaderModule>& sModule)
    {
        std::string importDef;
        
        switch (sModule->GetFamily()) {
        case ShaderModule::Family::Main: HL_EXCEPTION(true, "Not allowed to include Main modules!"); break;
        case ShaderModule::Family::Surface: importDef += "Surface"; break;
        case ShaderModule::Family::Compute: importDef += "Compute"; break;
        case ShaderModule::Family::Function: importDef += "Function"; break;
        break;
        }

        importDef += "/" + std::string(sModule->GetTag());
        
        return importDef;
    }
    
    template <typename... Args>
    inline std::string FormattedBuildError(
        const std::filesystem::path& file,
        std::size_t line,
        std::format_string<Args...> fmt,
        Args&&... args)
    {
        std::ostringstream stream;
        stream << "Error at line " << line << " in file: " << conversions::from_utf16_to_utf8(file.wstring()) << '\n';
        stream << std::format(fmt, std::forward<Args>(args)...) << '\n';
        return stream.str();
    }

    
    static const std::regex ifdefRegex(R"(^\s*#\s*ifdef\s+(\w+)\s*$)");
    static const std::regex endifRegex(R"(^\s*#\s*endif\s*$)");
    static const std::regex includeRegex(R"(^\s*#\s*include\s*(?:<([a-zA-Z0-9_./\-]+)>|"([a-zA-Z0-9_./\-]+)\")$)");
    static const std::regex importRegex(R"(^\s*@\s*import\s*(?:<([a-zA-Z0-9_./\-]+)>|"([a-zA-Z0-9_./\-]+)\")$)");
    static const std::regex registerRegex(R"(:\s*register\s*\(\s*([btus])(\d+)\s*\))");
    static const std::regex nameRegex(R"(\b([A-Za-z_][A-Za-z0-9_]*)\s+([A-Za-z_][A-Za-z0-9_]*)(\s*\[\s*\d+\s*\])?\s*;)");
    static const std::regex uniformRegex(R"(\b(cbuffer|uniform|UniformBuffer)\s+([A-Za-z_][A-Za-z0-9_]*)(\s*\[\s*\d+\s*\])?\s*)");

    static const std::regex openBraceRegex(R"(\s*\{)");
    static const std::regex closeBraceRegex(R"(\s*\})");

    inline std::string ResolveShaderFile(const std::filesystem::path& shaderPath, std::uint32_t space, const std::vector<Ref<ShaderModule>> shaderModules, const std::unordered_map<Hash128Key, std::uint32_t, Hash128KeyHasher, Hash128KeyEqual>& shaderModuleHashToSet, BindingLink& bindingLink, std::vector<std::string> preprocessorDefines, std::unordered_set<std::string>& included)
    {
        std::ifstream file(shaderPath);
        if (!file) {
            HL_EXCEPTION(true, "Unable to open file: " + conversions::from_utf16_to_utf8(shaderPath.wstring()))
        } 
        
        std::stringstream sstream;

        std::unordered_map<char, uint32> reservedSlots;
        std::unordered_map<char, uint32> currentBinding;

        uint32 braceLevel = 0;
        
        int ignoreDepth = 0;
        std::string line;
        size_t lineNumber = 0;
        bool inBlockComment = false;

        sstream << "#pragma pack_matrix(column_major)\n";

        while (std::getline(file, line)) {
            lineNumber++;
            if (line.empty()) continue;
            std::smatch match;
            
            size_t pos = line.find("//");
            if (pos != std::string::npos)
                line = line.substr(0, pos);


            if (inBlockComment) {
                size_t end = line.find("*/");
                if (end != std::string::npos) {
                    line = line.substr(end + 2);
                    inBlockComment = false;
                } else {
                    continue;
                }
            }

            size_t start = line.find("/*");
            if (start != std::string::npos) {
                size_t end = line.find("*/", start + 2);
                if (end != std::string::npos) {
                    line.erase(start, end - start + 2);
                } else {
                    line = line.substr(0, start);
                    inBlockComment = true;
                }
            }

            
            if (std::regex_match(line, match, ifdefRegex)) {
                std::string macro = match[1];

                if (ignoreDepth > 0 || 
                    std::find(preprocessorDefines.begin(), preprocessorDefines.end(), macro) == preprocessorDefines.end()) {
                    ignoreDepth++;
                }
                continue;
            } else if (std::regex_match(line, endifRegex)) {
                if (ignoreDepth > 0) ignoreDepth--;
                continue;
            }

            if (ignoreDepth > 0) continue;
            

            braceLevel += std::count(line.begin(), line.end(), '{');
            braceLevel -= std::count(line.begin(), line.end(), '}');

            if (braceLevel > 0) 
            {
                sstream << line << '\n';
                continue;
            }

            if (std::regex_match(line, match, includeRegex)) {
                std::string captured = match[1].matched ? match[1].str() : match[2].str();
                
                std::filesystem::path path = captured;
                path = "Shaders" / path;
                if (!std::filesystem::exists(path)) HL_EXCEPTION(true, FormattedBuildError(conversions::from_utf16_to_utf8(shaderPath.wstring()), lineNumber, "Included file: {} does not exist!", captured));                
                
                if (included.contains(captured)) continue;

                sstream << "// --- BEGIN #include " << '"' << captured << "\"\n";
                sstream << ResolveShaderFile(path, space, shaderModules, shaderModuleHashToSet, bindingLink, preprocessorDefines, included) << '\n';
                sstream << "// --- END #include " << '"' << captured << "\"\n";

                included.insert(captured);

                continue;
            } else if (std::regex_match(line, match, importRegex)) {
                std::string captured = match[1].matched ? match[1].str() : match[2].str();
                if (included.contains(captured)) continue;
        
                auto smod = std::find_if(shaderModules.begin(), shaderModules.end(), [captured](Ref<ShaderModule> x) { 
                    if (x->GetFamily() == ShaderModule::Family::Main) return false;
                    return ConstructImport(x) == captured;
                });
                
                HL_EXCEPTION(smod == shaderModules.end(), FormattedBuildError(conversions::from_u8string(shaderPath.u8string()), lineNumber, "Imported module: {} does not exist!", captured));
                // HL_EXCEPTION((*smod)->GetFamily() == ShaderModule::Family::Main, FormatedBuildError("Imported module: " + captured + " is of family main. Modules of family type Main may not be imported!", conversions::from_u8string(shaderPath.u8string()), lineNumber));

                std::filesystem::path path = "Shaders" / (*smod)->GetPath();
                if (!std::filesystem::exists(path)) HL_EXCEPTION(true, FormattedBuildError(conversions::from_u8string(shaderPath.u8string()), lineNumber, "Imported module: {} is importing a {} that does not exist!", captured, conversions::from_u8string(path.u8string())));                
                Hash128Key key((*smod)->GetHash().low64, (*smod)->GetHash().high64);
                
                sstream << "// --- BEGIN @import " << '"' << captured << "\"\n";
                sstream << ResolveShaderFile(path, shaderModuleHashToSet.at(key), shaderModules, shaderModuleHashToSet, bindingLink, preprocessorDefines, included);
                sstream << "// --- END @import " << '"' << captured << "\"\n";
                included.insert(captured);
                continue;
            } else if (std::regex_search(line, match, registerRegex))
            {
                std::string replacement = ": register(" + match[1].str() + match[2].str() +
                                ", space" + std::to_string(space) + ")";

                auto slot = std::stoi(match[2].str());
                
                if (reservedSlots.contains(match[1].str()[0]))
                {
                    HL_EXCEPTION(true, FormattedBuildError(conversions::from_u8string(shaderPath.u8string()), lineNumber, "Register {} is already reserved!", match[2].str()));
                }

                reservedSlots[match[1].str()[0]] = slot;
            
                sstream << line.substr(0, match.position()) 
                       + replacement 
                       + line.substr(match.position() + match.length())
                       + '\n';
                       
                continue;
            } else if (std::regex_search(line, match, nameRegex))
            {
                std::string type = match[1].str();
                std::string name = match[2].str();
                

                BindingType bindingType = BindingType::Unknown;

                std::string bindTypeStr = "";
                std::string realType = "";

                if (type == "Texture2D")
                {
                    bindingType = BindingType::Image;
                    realType = "Texture2D";
                    bindTypeStr = "t";
                }
                else if (type == "Texture2DSampler")
                {
                    bindingType = BindingType::CombinedTextureSampler2D;
                    realType = "Texture2DSampler";
                    bindTypeStr = "t";
                }
                else if (type == "Sampler" || type == "SamplerState")
                {
                    bindingType = BindingType::Sampler;
                    realType = "SamplerState";
                    bindTypeStr = "s";
                } else
                {
                    HL_EXCEPTION(true, FormattedBuildError(conversions::from_u8string(shaderPath.u8string()), lineNumber, "Unknown binding type {}! At line {} in {}", type, lineNumber, conversions::from_u8string(shaderPath.u8string())));
                }

                while (reservedSlots.contains(bindTypeStr[0]) && reservedSlots[bindTypeStr[0]] == currentBinding[bindTypeStr[0]]) currentBinding[bindTypeStr[0]]++;
                

                bindingLink.SetBinding(name, bindingType, space, currentBinding[bindTypeStr[0]]);

                sstream << realType << " " << name 
                    << ": register(" << bindTypeStr << std::to_string(currentBinding[bindTypeStr[0]]) << ", space" << std::to_string(space) << ")"
                    << line.substr(match.position() + match.length()) << ";\n";
                
                currentBinding[bindTypeStr[0]]++;
                continue;
                
            } else if (std::regex_search(line, match, uniformRegex))
            {
                std::string name = match[2].str();
                
                while (reservedSlots.contains('b') && reservedSlots['b'] == currentBinding['b']) currentBinding['b']++;

                bindingLink.SetBinding(name, BindingType::UniformBuffer, space, currentBinding['b']);

                sstream << "cbuffer " << name << ": register(b" << std::to_string(currentBinding['b']) << ", space" << std::to_string(space) << ")"
                       << line.substr(match.position() + match.length()) << '\n';
                
                currentBinding['b']++;
                continue;
            }
            
            sstream << line << '\n';
        }

        return sstream.str();
    }
    
    inline std::string ResolveShaderModule(const Ref<ShaderModule>& mainModule, const std::vector<Ref<ShaderModule>> shaderModules, const std::unordered_map<Hash128Key, std::uint32_t, Hash128KeyHasher, Hash128KeyEqual>& shaderModuleHashToSet, BindingLink& bindingLink, std::vector<std::string> preprocessorDefines)
    {        
        
        HL_EXCEPTION(mainModule->GetPath().empty(), "Path is empty, IR code is not supported yet!");
        
        std::filesystem::path shaderPartLocation = "Shaders" / mainModule->GetPath(); 
        
        std::unordered_set<std::string> included;
        Hash128Key x(mainModule->GetHash().low64, mainModule->GetHash().high64);
        return ResolveShaderFile(shaderPartLocation, shaderModuleHashToSet.at(x), shaderModules, shaderModuleHashToSet, bindingLink, preprocessorDefines, included);
    }

    inline SpvReflectDescriptorType GetSPVReflDescType(BindingType type)
    {
        switch(type)
        {
            case BindingType::CombinedTextureSampler2D: return SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case BindingType::UniformBuffer: return SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case BindingType::Image: return SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case BindingType::Sampler: return SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER;
            default: break;
        };

        HL_EXCEPTION(true, "Unknown BindingType!");
    }

    inline std::string_view constexpr GetElementNameFromReflectedType(const SpvReflectInterfaceVariable* var, const InputElement& elem)
    {
        const auto& num = var->numeric;

        if (num.matrix.column_count > 1 || (
            elem.type == ShaderDataType::MatrixFloat2x2 ||
            elem.type == ShaderDataType::MatrixFloat3x3 ||
            elem.type == ShaderDataType::MatrixFloat4x4
        )) {

            switch (var->format) {
                case SPV_REFLECT_FORMAT_R32G32_SFLOAT: return StringFromShaderDataType(ShaderDataType::MatrixFloat2x2);
                case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT: return StringFromShaderDataType(ShaderDataType::MatrixFloat3x3);
                case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT: return StringFromShaderDataType(ShaderDataType::MatrixFloat4x4);
                default: return StringFromShaderDataType(ShaderDataType::Unknown);
            }
        }

        switch (var->format) {
            // Float32
            case SPV_REFLECT_FORMAT_R32_SFLOAT:           return StringFromShaderDataType(ShaderDataType::Float32);
            case SPV_REFLECT_FORMAT_R32G32_SFLOAT:        return StringFromShaderDataType(ShaderDataType::Float32_2);
            case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:     return StringFromShaderDataType(ShaderDataType::Float32_3);
            case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:  return StringFromShaderDataType(ShaderDataType::Float32_4);

            // Float16
            case SPV_REFLECT_FORMAT_R16_SFLOAT:           return StringFromShaderDataType(ShaderDataType::Float16);
            case SPV_REFLECT_FORMAT_R16G16_SFLOAT:        return StringFromShaderDataType(ShaderDataType::Float16_2);
            case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:  return StringFromShaderDataType(ShaderDataType::Float16_4);

            // Int32
            case SPV_REFLECT_FORMAT_R32_SINT:
                switch (elem.type) {
                    case ShaderDataType::Int32:    return StringFromShaderDataType(ShaderDataType::Int32);
                    case ShaderDataType::Int8:     return StringFromShaderDataType(ShaderDataType::Int8);
                    case ShaderDataType::NormInt8: return StringFromShaderDataType(ShaderDataType::NormInt8);
                    default:                       return "Unknown (R32_SINT)!";
                }

            case SPV_REFLECT_FORMAT_R32G32_SINT:
                if (elem.type == ShaderDataType::Int32_2)    return StringFromShaderDataType(ShaderDataType::Int32_2);
                if (elem.type == ShaderDataType::Int8_2)     return StringFromShaderDataType(ShaderDataType::Int8_2);
                if (elem.type == ShaderDataType::NormInt8_2) return StringFromShaderDataType(ShaderDataType::NormInt8_2);
                return "Unknown (R32G32_SINT)!";

            case SPV_REFLECT_FORMAT_R32G32B32_SINT:
                return StringFromShaderDataType(ShaderDataType::Int32_3);

            case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
                if (elem.type == ShaderDataType::Int32_4)    return StringFromShaderDataType(ShaderDataType::Int32_4);
                if (elem.type == ShaderDataType::Int8_4)     return StringFromShaderDataType(ShaderDataType::Int8_4);
                if (elem.type == ShaderDataType::NormInt8_4) return StringFromShaderDataType(ShaderDataType::NormInt8_4);
                return "Unknown (R32G32B32A32_SINT)!";

            // Int16
            case SPV_REFLECT_FORMAT_R16_SINT:
                if (elem.type == ShaderDataType::Int16)      return StringFromShaderDataType(ShaderDataType::Int16);
                if (elem.type == ShaderDataType::Int8)       return StringFromShaderDataType(ShaderDataType::Int8);
                if (elem.type == ShaderDataType::NormInt8)   return StringFromShaderDataType(ShaderDataType::NormInt8);
                if (elem.type == ShaderDataType::NormInt16)  return StringFromShaderDataType(ShaderDataType::NormInt16);
                return "Unknown (R16_SINT)!";

            case SPV_REFLECT_FORMAT_R16G16_SINT:
                if (elem.type == ShaderDataType::Int16_2)    return StringFromShaderDataType(ShaderDataType::Int16_2);
                if (elem.type == ShaderDataType::Int8_2)     return StringFromShaderDataType(ShaderDataType::Int8_2);
                if (elem.type == ShaderDataType::NormInt8_2) return StringFromShaderDataType(ShaderDataType::NormInt8_2);
                if (elem.type == ShaderDataType::NormInt16_2)return StringFromShaderDataType(ShaderDataType::NormInt16_2);
                return "Unknown (R16G16_SINT)!";

            case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:
                if (elem.type == ShaderDataType::Int16_4)    return StringFromShaderDataType(ShaderDataType::Int16_4);
                if (elem.type == ShaderDataType::Int8_4)     return StringFromShaderDataType(ShaderDataType::Int8_4);
                if (elem.type == ShaderDataType::NormInt8_4) return StringFromShaderDataType(ShaderDataType::NormInt8_4);
                if (elem.type == ShaderDataType::NormInt16_4)return StringFromShaderDataType(ShaderDataType::NormInt16_4);
                return "Unknown (R16G16B16A16_SINT)!";

            // UInt32
            case SPV_REFLECT_FORMAT_R32_UINT:
                if (elem.type == ShaderDataType::UInt32)     return StringFromShaderDataType(ShaderDataType::UInt32);
                if (elem.type == ShaderDataType::UInt8)      return StringFromShaderDataType(ShaderDataType::UInt8);
                if (elem.type == ShaderDataType::NormUInt8)  return StringFromShaderDataType(ShaderDataType::NormUInt8);
                return "Unknown (R32_UINT)!";

            case SPV_REFLECT_FORMAT_R32G32_UINT:
                if (elem.type == ShaderDataType::UInt32_2)   return StringFromShaderDataType(ShaderDataType::UInt32_2);
                if (elem.type == ShaderDataType::UInt8_2)    return StringFromShaderDataType(ShaderDataType::UInt8_2);
                if (elem.type == ShaderDataType::NormUInt8_2)return StringFromShaderDataType(ShaderDataType::NormUInt8_2);
                return "Unknown (R32G32_UINT)!";

            case SPV_REFLECT_FORMAT_R32G32B32_UINT:
                return StringFromShaderDataType(ShaderDataType::UInt32_3);

            case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
                if (elem.type == ShaderDataType::UInt32_4)   return StringFromShaderDataType(ShaderDataType::UInt32_4);
                if (elem.type == ShaderDataType::UInt8_4)    return StringFromShaderDataType(ShaderDataType::UInt8_4);
                if (elem.type == ShaderDataType::NormUInt8_4)return StringFromShaderDataType(ShaderDataType::NormUInt8_4);
                return "Unknown (R32G32B32A32_UINT)!";

            // UInt16
            case SPV_REFLECT_FORMAT_R16_UINT:
                if (elem.type == ShaderDataType::UInt16)     return StringFromShaderDataType(ShaderDataType::UInt16);
                if (elem.type == ShaderDataType::UInt8)      return StringFromShaderDataType(ShaderDataType::UInt8);
                if (elem.type == ShaderDataType::NormUInt8)  return StringFromShaderDataType(ShaderDataType::NormUInt8);
                return "Unknown (R16_UINT)!";

            case SPV_REFLECT_FORMAT_R16G16_UINT:
                if (elem.type == ShaderDataType::UInt16_2)   return StringFromShaderDataType(ShaderDataType::UInt16_2);
                if (elem.type == ShaderDataType::UInt8_2)    return StringFromShaderDataType(ShaderDataType::UInt8_2);
                if (elem.type == ShaderDataType::NormUInt8_2)return StringFromShaderDataType(ShaderDataType::NormUInt8_2);
                return "Unknown (R16G16_UINT)!";

            case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:
                if (elem.type == ShaderDataType::UInt16_4)   return StringFromShaderDataType(ShaderDataType::UInt16_4);
                if (elem.type == ShaderDataType::UInt8_4)    return StringFromShaderDataType(ShaderDataType::UInt8_4);
                if (elem.type == ShaderDataType::NormUInt8_4)return StringFromShaderDataType(ShaderDataType::NormUInt8_4);
                return "Unknown (R16G16B16A16_UINT)!";

            default:
                return "UnknownFormat!";
        }
    }

    inline bool MatchesInputElement(const SpvReflectInterfaceVariable* var, const InputElement& elem)
    {
        const auto& num = var->numeric;

        if (num.matrix.column_count > 1) {
            switch (num.matrix.column_count) {
                case 2: return elem.type == ShaderDataType::MatrixFloat2x2;
                case 3: return elem.type == ShaderDataType::MatrixFloat3x3;
                case 4: return elem.type == ShaderDataType::MatrixFloat4x4;
                default: return false;
            }
        }

        switch (var->format) {
            // Float32
            case SPV_REFLECT_FORMAT_R32_SFLOAT:           return elem.type == ShaderDataType::Float32;
            case SPV_REFLECT_FORMAT_R32G32_SFLOAT:        return elem.type == ShaderDataType::Float32_2;
            case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:     return elem.type == ShaderDataType::Float32_3;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:  return elem.type == ShaderDataType::Float32_4;

            // Float16
            case SPV_REFLECT_FORMAT_R16_SFLOAT:           return elem.type == ShaderDataType::Float16;
            case SPV_REFLECT_FORMAT_R16G16_SFLOAT:        return elem.type == ShaderDataType::Float16_2;
            case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:  return elem.type == ShaderDataType::Float16_4;

            // Int32
            case SPV_REFLECT_FORMAT_R32_SINT:             return elem.type == ShaderDataType::Int32   || elem.type == ShaderDataType::Int8   || elem.type == ShaderDataType::NormInt8;
            case SPV_REFLECT_FORMAT_R32G32_SINT:          return elem.type == ShaderDataType::Int32_2 || elem.type == ShaderDataType::Int8_2 || elem.type == ShaderDataType::NormInt8_2;
            case SPV_REFLECT_FORMAT_R32G32B32_SINT:       return elem.type == ShaderDataType::Int32_3;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:    return elem.type == ShaderDataType::Int32_4 || elem.type == ShaderDataType::Int8_4 || elem.type == ShaderDataType::NormInt8_4;

            // Int16
            case SPV_REFLECT_FORMAT_R16_SINT:             return elem.type == ShaderDataType::Int16   || elem.type == ShaderDataType::Int8   || elem.type == ShaderDataType::NormInt8   || elem.type == ShaderDataType::NormInt16;
            case SPV_REFLECT_FORMAT_R16G16_SINT:          return elem.type == ShaderDataType::Int16_2 || elem.type == ShaderDataType::Int8_2 || elem.type == ShaderDataType::NormInt8_2 || elem.type == ShaderDataType::NormInt16_2;
            case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:    return elem.type == ShaderDataType::Int16_4 || elem.type == ShaderDataType::Int8_4 || elem.type == ShaderDataType::NormInt8_4 || elem.type == ShaderDataType::NormInt16_4;

            // UInt32
            case SPV_REFLECT_FORMAT_R32_UINT:             return elem.type == ShaderDataType::UInt32   || elem.type == ShaderDataType::UInt8   || elem.type == ShaderDataType::NormUInt8;
            case SPV_REFLECT_FORMAT_R32G32_UINT:          return elem.type == ShaderDataType::UInt32_2 || elem.type == ShaderDataType::UInt8_2 || elem.type == ShaderDataType::NormUInt8_2;
            case SPV_REFLECT_FORMAT_R32G32B32_UINT:       return elem.type == ShaderDataType::UInt32_3;
            case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:    return elem.type == ShaderDataType::UInt32_4 || elem.type == ShaderDataType::UInt8_4 || elem.type == ShaderDataType::NormUInt8_4;

            // UInt16
            case SPV_REFLECT_FORMAT_R16_UINT:             return elem.type == ShaderDataType::UInt16   || elem.type == ShaderDataType::UInt8   || elem.type == ShaderDataType::NormUInt8;
            case SPV_REFLECT_FORMAT_R16G16_UINT:          return elem.type == ShaderDataType::UInt16_2 || elem.type == ShaderDataType::UInt8_2 || elem.type == ShaderDataType::NormUInt8_2;
            case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:    return elem.type == ShaderDataType::UInt16_4 || elem.type == ShaderDataType::UInt8_4 || elem.type == ShaderDataType::NormUInt8_4;

            default: return false;
        }
    }


    inline bool ValidateInputLayoutsShader(const std::vector<char>& code, const InputLayouts& inputLayouts)
    {
        std::unordered_map<std::string, InputElement> elements;
        for (auto& inputLayout : inputLayouts)
            for (auto& elem : inputLayout)
                elements.emplace(elem.name, elem);

        SpvReflectShaderModule module;
        SpvReflectResult res = spvReflectCreateShaderModule(code.size(), code.data(), &module);
        if (res != SPV_REFLECT_RESULT_SUCCESS)
            return false;

        size_t elemCount = 0;

        for (uint32_t i = 0; i < module.input_variable_count; ++i)
        {
            const SpvReflectInterfaceVariable* input = module.input_variables[i];
            if (input->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
                continue;

            uint32_t numCols = input->numeric.matrix.column_count;
            if (numCols == 0) numCols = 1;

            for (uint32_t col = 0; col < numCols; ++col)
            {
                if (elemCount >= elements.size())
                {
                    std::cout << (std::format(
                        "Shader input '{}' requires more elements than provided",
                        input->name ? input->name : "<unnamed>"
                    ).c_str()) << std::endl;
                    spvReflectDestroyShaderModule(&module);
                    return false;
                }

                if (!elements.contains(input->name))
                    continue;

                const auto& provided = elements[input->name];

                if (!MatchesInputElement(input, provided))
                {
                    std::cout << (
                        std::format("Shader input '{}' column {} does not match provided layout.\n\tExcepted {} found {}!",
                        input->name ? input->name : "<unnamed>", col,
                        StringFromShaderDataType(provided.type),
                        GetElementNameFromReflectedType(input, provided)
                    ).c_str()) << std::endl;
                    spvReflectDestroyShaderModule(&module);
                    return false;
                }

                elemCount += numCols;
            }
        }

        spvReflectDestroyShaderModule(&module);
        return true;
    }

    // * OLD VALIDATION CODE
    // inline bool ValidateShader(const std::vector<uint8_t>& code, const std::vector<BindingLayout>& bindingLayouts)
    // {
        
    //     SpvReflectShaderModule module;
    //     SpvReflectResult res = spvReflectCreateShaderModule(code.size(), code.data(), &module);
    //     if (res != SPV_REFLECT_RESULT_SUCCESS)
    //         return false;

    //     for (uint32_t i = 0; i < module.descriptor_binding_count; ++i)
    //     {
    //         const SpvReflectDescriptorBinding& binding = module.descriptor_bindings[i];

    //         if (binding.set >= bindingLayouts.size())
    //         {
    //             std::cout << std::format("Shader uses set {} which does not exist in pipeline layout", binding.set) << std::endl;
    //             spvReflectDestroyShaderModule(&module);
    //             return false;
    //         }

    //         const auto& setLayout = bindingLayouts[binding.set];

    //         bool found = false;
    //         for (const auto& layoutBinding : setLayout.m_bindings)
    //         {
    //             if (layoutBinding.slot == binding.binding)
    //             {
    //                 if (GetSPVReflDescType(layoutBinding.type) == binding.descriptor_type)
    //                 {
    //                     found = true;
    //                     break;
    //                 }                    
    //             }
    //         }


    //         if (!found)
    //         {
    //             std::cout << std::format("Shader uses binding not present in set {} : binding {}", binding.set, binding.binding) << std::endl;
    //             spvReflectDestroyShaderModule(&module);
    //             return false;
    //         }
    //     }

    //     spvReflectDestroyShaderModule(&module);
    //     return true;
    // }

    inline BindingType GetBindingTypeFromSPVRefl(SpvReflectDescriptorType type)
    {
        switch(type)
        {
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return BindingType::CombinedTextureSampler2D;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return BindingType::UniformBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return BindingType::Image;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return BindingType::Sampler;
            default: break;
        };

        HL_EXCEPTION(true, "Unknown BindingType!");
    }

    inline bool ReflectShaderBindings(const std::vector<char>& code, std::vector<BindingLayout>& bindingLayouts)
    {
        SpvReflectShaderModule module;
        SpvReflectResult res = spvReflectCreateShaderModule(code.size(), code.data(), &module);
        if (res != SPV_REFLECT_RESULT_SUCCESS)
            return false;

        bindingLayouts.clear();

        uint32_t maxSet = 0;
        if (module.descriptor_binding_count > 0)
        {
            for (uint32_t i = 0; i < module.descriptor_binding_count; ++i)
            {
                const SpvReflectDescriptorBinding& binding = module.descriptor_bindings[i];
                if (binding.set > maxSet)
                    maxSet = binding.set;
            }
            // bindingLayouts.resize(maxSet + 1);

            for (uint32_t i = 0; i < maxSet + 1; ++i)
            {
                bindingLayouts.push_back(BindingLayout({}));
            }
        }

        for (uint32_t i = 0; i < module.descriptor_binding_count; ++i)
        {
            const SpvReflectDescriptorBinding& binding = module.descriptor_bindings[i];

            BindingInfo bindingInfo;
            bindingInfo.slot = binding.binding;
            bindingInfo.type = GetBindingTypeFromSPVRefl(binding.descriptor_type);
            bindingInfo.count = binding.count;

            bindingLayouts[binding.set].m_bindings.push_back(bindingInfo);
        }

        spvReflectDestroyShaderModule(&module);
        return true;
    }
}