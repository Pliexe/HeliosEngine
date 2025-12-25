#include "VkGraphicsPipeline.h"
#include "Helios/Core/Application.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Logger.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/GraphicsPipeline.h"
#include "Helios/Resources/ShaderModule.h"
#include "Platform/Vulkan/GraphicsPipeline/VkDescriptorSetLayoutCache.h"
#include "Platform/Vulkan/Shader/VkGeometryShader.h"
#include "Platform/Vulkan/Shader/VkPixelShader.h"
#include "Platform/Vulkan/Shader/VkVertexShader.h"
#include "Platform/Vulkan/VkContext.h"
#include <optional>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    // bool VkGraphicsPipeline::SetTexture2D(LayoutSet set, uint32_t slot, Ref<Texture2D> texture2d)
    // {
    //     if (set == LayoutSet::Material)
    //     {
    //         HL_MESSAGE("Binding in set can only be used by material exclusivly!");
    //         return false;
    //     }
        
    //     auto llit = m_logicalLayouts.find(set);
    //     if (llit == m_logicalLayouts.end())
    //         return false;

    //     auto& bindings = llit->second.m_bindings;
    //     // TODO: This is slow, change it later.
    //     auto it = std::find_if(bindings.begin(), bindings.end(), [slot](BindingInfo& x) { return x.slot == slot; });

    //     if (it == bindings.end())
    //     {
    //         std::string str = std::format("Binding in set ({}) at slot ({}) does not exist in the layout used by this pipeline!", static_cast<uint32_t>(set), slot);
    //         HL_MESSAGE(str.c_str());
    //         return false;
    //     }

    //     if (it->type != BindingType::Texture2D) 
    //     {
    //         std::string str = std::format("Binding type missmatch in set ({}) at slot ({})! Tried to set Texture2D but {} was received!", static_cast<uint32_t>(set), slot, to_string(it->type));
    //         HL_MESSAGE(str.c_str());
    //         return false;
    //     }

    //     m_bindingMap[set].SetResource(slot, texture2d);

    //     return true;
    // }
    // bool VkGraphicsPipeline::SetSampler(LayoutSet set, uint32_t slot, Ref<Sampler> sampler)
    // {
    //     if (set == LayoutSet::Material)
    //     {
    //         HL_MESSAGE("Binding in set can only be used by material exclusivly!");
    //         return false;
    //     }
        
    //     auto llit = m_logicalLayouts.find(set);
    //     if (llit == m_logicalLayouts.end())
    //         return false;

    //     auto& bindings = llit->second.m_bindings;
    //     // TODO: This is slow, change it later.
    //     auto it = std::find_if(bindings.begin(), bindings.end(), [slot](BindingInfo& x) { return x.slot == slot; });

    //     if (it == bindings.end())
    //     {
    //         std::string str = std::format("Binding in set ({}) at slot ({}) does not exist in the layout used by this pipeline!", static_cast<uint32_t>(set), slot);
    //         HL_MESSAGE(str.c_str());
    //         return false;
    //     }

    //     if (it->type != BindingType::Sampler) 
    //     {
    //         std::string str = std::format("Binding type missmatch in set ({}) at slot ({})! Tried to set Sampler but {} was received!", static_cast<uint32_t>(set), slot, to_string(it->type));
    //         HL_MESSAGE(str.c_str());
    //         return false;
    //     }

    //     m_bindingMap[set].SetResource(slot, sampler);

    //     return true;
    // }
    // bool VkGraphicsPipeline::SetUniformBuffer(LayoutSet set, uint32_t slot, Ref<UnsafeUniformBuffer> uniformBuffer)
    // {
    //     if (set == LayoutSet::Material)
    //     {
    //         HL_MESSAGE("Binding in set can only be used by material exclusivly!");
    //         return false;
    //     }
        
    //     auto llit = m_logicalLayouts.find(set);
    //     if (llit == m_logicalLayouts.end())
    //         return false;

    //     auto& bindings = llit->second.m_bindings;
    //     // TODO: This is slow, change it later.
    //     auto it = std::find_if(bindings.begin(), bindings.end(), [slot](BindingInfo& x) { return x.slot == slot; });

    //     if (it == bindings.end())
    //     {
    //         std::string str = std::format("Binding in set ({}) at slot ({}) does not exist in the layout used by this pipeline!", static_cast<uint32_t>(set), slot);
    //         HL_MESSAGE(str.c_str());
    //         return false;
    //     }

    //     if (it->type != BindingType::UniformBuffer) 
    //     {
    //         std::string str = std::format("Binding type missmatch in set ({}) at slot ({})! Tried to set UniformBuffer but {} was received!", static_cast<uint32_t>(set), slot, to_string(it->type));
    //         HL_MESSAGE(str.c_str());
    //         return false;
    //     }

    //     m_bindingMap[set].SetResource(slot, uniformBuffer);

    //     return true;
    // }
    
    
    // bool VkGraphicsPipeline::SetMaterial(Ref<MaterialInstance> materialInstance)
    // {
    //     return false;
    // }
    
    struct GeneratedVertexInputInfo {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    };

    static VkFormat GetVkFormat(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float16: return VK_FORMAT_R16_SFLOAT;
            case ShaderDataType::Float16_2: return VK_FORMAT_R16G16_SFLOAT;
            case ShaderDataType::Float16_4: return VK_FORMAT_R16G16B16A16_SFLOAT;
            case ShaderDataType::Float32: return VK_FORMAT_R32_SFLOAT;
            case ShaderDataType::Float32_2: return VK_FORMAT_R32G32_SFLOAT;
            case ShaderDataType::Float32_3: return VK_FORMAT_R32G32B32_SFLOAT;
            case ShaderDataType::Float32_4: return VK_FORMAT_R32G32B32A32_SFLOAT;

            case ShaderDataType::Int8: return VK_FORMAT_R8_SINT;
            case ShaderDataType::Int8_2: return VK_FORMAT_R8G8_SINT;
            case ShaderDataType::Int8_4: return VK_FORMAT_R8G8B8A8_SINT;

            case ShaderDataType::UInt8: return VK_FORMAT_R8_UINT;
            case ShaderDataType::UInt8_2: return VK_FORMAT_R8G8_UINT;
            case ShaderDataType::UInt8_4: return VK_FORMAT_R8G8B8A8_UINT;

            case ShaderDataType::Int16: return VK_FORMAT_R16_SINT;
            case ShaderDataType::Int16_2: return VK_FORMAT_R16G16_SINT;
            case ShaderDataType::Int16_4: return VK_FORMAT_R16G16B16A16_SINT;

            case ShaderDataType::UInt16: return VK_FORMAT_R16_UINT;
            case ShaderDataType::UInt16_2: return VK_FORMAT_R16G16_UINT;
            case ShaderDataType::UInt16_4: return VK_FORMAT_R16G16B16A16_UINT;

            case ShaderDataType::Int32: return VK_FORMAT_R32_SINT;
            case ShaderDataType::Int32_2: return VK_FORMAT_R32G32_SINT;
            case ShaderDataType::Int32_3: return VK_FORMAT_R32G32B32_SINT;
            case ShaderDataType::Int32_4: return VK_FORMAT_R32G32B32A32_SINT;

            case ShaderDataType::UInt32: return VK_FORMAT_R32_UINT;
            case ShaderDataType::UInt32_2: return VK_FORMAT_R32G32_UINT;
            case ShaderDataType::UInt32_3: return VK_FORMAT_R32G32B32_UINT;
            case ShaderDataType::UInt32_4: return VK_FORMAT_R32G32B32A32_UINT;

            case ShaderDataType::NormInt8: return VK_FORMAT_R8_SNORM;
            case ShaderDataType::NormInt8_2: return VK_FORMAT_R8G8_SNORM;
            case ShaderDataType::NormInt8_4: return VK_FORMAT_R8G8B8A8_SNORM;

            case ShaderDataType::NormUInt8: return VK_FORMAT_R8_UNORM;
            case ShaderDataType::NormUInt8_2: return VK_FORMAT_R8G8_UNORM;
            case ShaderDataType::NormUInt8_4: return VK_FORMAT_R8G8B8A8_UNORM;

            case ShaderDataType::NormInt16: return VK_FORMAT_R16_SNORM;
            case ShaderDataType::NormInt16_2: return VK_FORMAT_R16G16_SNORM;
            case ShaderDataType::NormInt16_4: return VK_FORMAT_R16G16B16A16_SNORM;

            case ShaderDataType::NormUInt16: return VK_FORMAT_R16_UNORM;
            case ShaderDataType::NormUInt16_2: return VK_FORMAT_R16G16_UNORM;
            case ShaderDataType::NormUInt16_4: return VK_FORMAT_R16G16B16A16_UNORM;

            default: return VK_FORMAT_UNDEFINED;
        }
    }

    GeneratedVertexInputInfo CreateVkVertexInputState(const InputLayouts& inputLayouts) {
        GeneratedVertexInputInfo result;
        uint32_t location = 0;

        for (uint32_t bindingIndex = 0; bindingIndex < inputLayouts.size(); ++bindingIndex) {
            const auto& layout = inputLayouts[bindingIndex];
            const auto& elements = layout.GetElements();
            InputClassification classification = layout.GetInputClassification();

            VkVertexInputBindingDescription binding{};
            binding.binding = bindingIndex;
            binding.inputRate = (classification == InputClassification::PerInstanceData) 
                            ? VK_VERTEX_INPUT_RATE_INSTANCE 
                            : VK_VERTEX_INPUT_RATE_VERTEX;

            uint32_t offset = 0;

            for (const auto& elem : elements) {
                if (elem.type == ShaderDataType::MatrixFloat2x2 ||
                    elem.type == ShaderDataType::MatrixFloat3x3 ||
                    elem.type == ShaderDataType::MatrixFloat4x4)
                {
                    int rows = 0;
                    int rowWidth = 0;
                    switch (elem.type) {
                        case ShaderDataType::MatrixFloat2x2: rows = 2; rowWidth = 2; break;
                        case ShaderDataType::MatrixFloat3x3: rows = 3; rowWidth = 3; break;
                        case ShaderDataType::MatrixFloat4x4: rows = 4; rowWidth = 4; break;
                        default: break;
                    }

                    uint32_t rowSize = rowWidth * sizeof(float);
                    for (int i = 0; i < rows; ++i) {
                        VkVertexInputAttributeDescription attr{};
                        attr.binding = bindingIndex;
                        attr.location = location++;
                        attr.format = GetVkFormat(static_cast<ShaderDataType>(
                            static_cast<int>(ShaderDataType::Float32) + (rowWidth - 1)
                        ));
                        attr.offset = offset;
                        result.attributes.push_back(attr);
                        offset += rowSize;
                    }

                    binding.stride += rowSize * rows;
                } else {
                    VkFormat format = GetVkFormat(elem.type);
                    HL_EXCEPTION(format == VK_FORMAT_UNDEFINED, "Unknown or unsupported ShaderDataType in Vulkan input!");

                    VkVertexInputAttributeDescription attr{};
                    attr.binding = bindingIndex;
                    attr.location = location++;
                    attr.format = format;
                    attr.offset = offset;
                    result.attributes.push_back(attr);

                    offset += ShaderDataTypeSize(elem.type);
                    binding.stride += ShaderDataTypeSize(elem.type);
                }
            }

            result.bindings.push_back(binding);
        }

        result.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        result.vertexInputInfo.vertexBindingDescriptionCount = result.bindings.size();
        result.vertexInputInfo.pVertexBindingDescriptions = result.bindings.data();
        result.vertexInputInfo.vertexAttributeDescriptionCount = result.attributes.size();
        result.vertexInputInfo.pVertexAttributeDescriptions = result.attributes.data();

        return result;
    }

    VkPrimitiveTopology VkTopologyFromTopology(Topology topology)
    {
        switch (topology)
        {
            case Topology::PointList:      return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case Topology::LineList:       return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case Topology::LineStrip:      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case Topology::TriangleList:   return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case Topology::TriangleStrip:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            default:                      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // fallback safe default
        }
    }
    
    VkCullModeFlags VkCullModeFromCullMode(CullMode cullMode)
    {
        switch (cullMode) {
            case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
            case CullMode::Both: return VK_CULL_MODE_FRONT_AND_BACK;
            default: return VK_CULL_MODE_BACK_BIT;
        }
    }

    VkPolygonMode VkPolygonModeFromFillMode(FillMode fillMode)
    {
        switch (fillMode) {
        case FillMode::Solid:       return VK_POLYGON_MODE_FILL;
        case FillMode::Wireframe:  return VK_POLYGON_MODE_LINE;
        default: return VK_POLYGON_MODE_FILL;
        }
    }
    
    VkGraphicsPipeline::VkGraphicsPipeline(const Ref<VkGraphicsDevice>& device, const VkGraphicsPipelineSpecification& specification) 
    {
        m_device = device;
        
        HL_EXCEPTION(specification.base.vertexShader == nullptr, "VertexShader is not defined!");
        HL_EXCEPTION(specification.base.pixelShader == nullptr, "PixelShader is not defined!");

        HL_EXCEPTION(specification.subpass > specification.renderPassKey.subpassCount, "Subpass set to value that is larger than the given renderPass allows!");
        
        size_t maxSetUsed = std::max({
        specification.base.vertexShader->GetBindingLayouts().size(),
        specification.base.pixelShader->GetBindingLayouts().size(),
        specification.base.geometryShader == nullptr ? 0 : specification.base.geometryShader->GetBindingLayouts().size()
        });

        std::vector<VkLayoutSetKey> mergedLayouts;
        mergedLayouts.resize(maxSetUsed);
        
        {
            auto& vecLayouts = specification.base.vertexShader->GetBindingLayouts();
            auto& pixLayouts = specification.base.pixelShader->GetBindingLayouts();
            std::int32_t setIndex = maxSetUsed-1;
            bool pruneLast = true;
            while(setIndex >= 0)
            {            
                VkLayoutSetKey set;
                
                if (setIndex < vecLayouts.size() && vecLayouts[setIndex].m_bindings.size() > 0) {
                    set.vertex = vecLayouts[setIndex];
                };
                if (setIndex < pixLayouts.size() && pixLayouts[setIndex].m_bindings.size() > 0)
                    set.pixel = pixLayouts[setIndex];

                if (specification.base.geometryShader != nullptr) {
                    auto& geoLayouts = specification.base.geometryShader->GetBindingLayouts();
                    if (setIndex < geoLayouts.size() && geoLayouts[setIndex].m_bindings.size() > 0) 
                        set.geometry = geoLayouts[setIndex];
                }
                
                if (!set.vertex.has_value() && !set.pixel.has_value() && !set.geometry.has_value())
                {
                    if (pruneLast) {
                        setIndex--;
                        continue;
                    }
                } else pruneLast = false;

                if (set.vertex.has_value()) for (auto& vl : set.vertex->m_bindings) m_pipelineBindingLayout[setIndex][vl.type][vl.slot] = vl; 
                if (set.pixel.has_value()) for (auto& vl : set.pixel->m_bindings) m_pipelineBindingLayout[setIndex][vl.type][vl.slot] = vl; 
                if (set.geometry.has_value()) for (auto& vl : set.geometry->m_bindings) m_pipelineBindingLayout[setIndex][vl.type][vl.slot] = vl; 
                
                // This is yikes but i dont want to think of better solution rn
                auto getMatchedIndex = [&](const BindingLayout& layout, const BindingInfo& info)
                {
                    if (info.type == BindingType::Image)
                    {
                        for (std::size_t i = 0, n = layout.m_bindings.size(); i < n; ++i)
                        {
                            if (layout.m_bindings[i].slot == info.slot && layout.m_bindings[i].type == BindingType::Sampler)
                            {
                                if (layout.m_bindings[i].count == info.count)
                                    return (int)i;
                                else return -1;
                            }
                        }    
                        return -1;
                    }
                    return -1;
                };

                auto mergeTexSamps = [&](BindingLayout& layout)
                {
                    auto& bindings = layout.m_bindings;

                    for (auto it = bindings.begin(); it != bindings.end(); )
                    {
                        auto index = getMatchedIndex(bindings, *it);
                        if (index > 0)
                        {
                            bindings[index].type = BindingType::CombinedTextureSampler2D;
                            it = bindings.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                };
                
                // TODO: Who made that mess? You did Pliexe!
                if (set.vertex.has_value()) mergeTexSamps(set.vertex.value());
                if (set.pixel.has_value()) mergeTexSamps(set.pixel.value());
                if (set.geometry.has_value()) mergeTexSamps(set.geometry.value());

                mergedLayouts[setIndex] = set;

                setIndex--;
            }
        }

        // {
        //     std::uint32_t i = 0;
        //     for (auto& ml : mergedLayouts)
        //     {
        //         if (ml.vertex.has_value()) for (auto& vl : ml.vertex->m_bindings) m_pipelineBindingLayout[i][vl.type][vl.slot] = vl; 
        //         if (ml.pixel.has_value()) for (auto& vl : ml.pixel->m_bindings) m_pipelineBindingLayout[i][vl.type][vl.slot] = vl; 
        //         if (ml.geometry.has_value()) for (auto& vl : ml.geometry->m_bindings) m_pipelineBindingLayout[i][vl.type][vl.slot] = vl; 
        //         ++i;
        //     }
        // }

        {
            for (auto& [key, value] : specification.base.vertexShader->GetModuleToSlotMap())
                m_moduleToSetSlot[key].push_back(value);

            for (auto& [key, value] : specification.base.pixelShader->GetModuleToSlotMap())
                m_moduleToSetSlot[key].push_back(value);

            if (specification.base.geometryShader != nullptr)
            {
                for (auto& [key, value] : specification.base.geometryShader->GetModuleToSlotMap())
                    m_moduleToSetSlot[key].push_back(value);
            }
        }

        {
            std::uint32_t i = 0;
            for (auto& ml : mergedLayouts)
            {
                std::vector<BindingInfo> setBindings;

                auto& typeToSlot = m_pipelineBindingLayout[i];
                for (auto& [type, slotMap] : typeToSlot)
                {
                    for (auto& [slot, binding] : slotMap)
                    {
                        // Dont need to check both since merge should happen only once anyways
                        if (type == BindingType::Image)
                        {
                            auto it = typeToSlot[BindingType::Sampler].find(slot);
                            if (it != typeToSlot[BindingType::Sampler].end())
                            {
                                if (binding.count == it->second.count)
                                {
                                    setBindings.push_back(BindingInfo {
                                        .slot = slot,
                                        .type = BindingType::CombinedTextureSampler2D,
                                        .count = binding.count,
                                    });
                                    continue;
                                }

                            }
                        }
                        if (type == BindingType::Sampler)
                        {
                            auto it = typeToSlot[BindingType::Image].find(slot);
                            if (it != typeToSlot[BindingType::Image].end()) continue;
                        }
                        setBindings.push_back(binding);
                    }
                }

                m_mergedBindingLayouts.insert({i, BindingLayout(setBindings)});
                ++i;
            }
        }

        std::vector<VkDescriptorSetLayout> descSetLayouts;

        for (auto& lay : mergedLayouts)
        {
            auto ref = m_device->GetDescriptorSetLayoutCache().Acquire(lay);
            m_layouts.push_back(ref);
            descSetLayouts.push_back(ref->Get());
        }

        VkPipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pSetLayouts = descSetLayouts.data();
        layoutInfo.setLayoutCount = descSetLayouts.size();

        HL_EXCEPTION(vkCreatePipelineLayout(m_device->GetNativeDevice(), &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS, "Unable to create VkPipelineLayout!");

        auto vertexInput = CreateVkVertexInputState(specification.base.vertexShader->GetInputLayouts());

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = static_cast<VkVertexShader&>(*specification.base.vertexShader).GetHandle();
        vertShaderStageInfo.pName = "MainVS";
        shaderStages.push_back(vertShaderStageInfo);

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = static_cast<VkPixelShader&>(*specification.base.pixelShader).GetHandle();
        fragShaderStageInfo.pName = "MainPS";
        shaderStages.push_back(fragShaderStageInfo);

        if (specification.base.geometryShader != nullptr)
        {
            VkPipelineShaderStageCreateInfo geoShaderStageInfo{};
            geoShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            geoShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            geoShaderStageInfo.module = static_cast<VkGeometryShader&>(*specification.base.geometryShader).GetHandle();
            geoShaderStageInfo.pName = "MainGS";
            shaderStages.push_back(geoShaderStageInfo);
        }
        
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = VkTopologyFromTopology(specification.base.topology);
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VkPolygonModeFromFillMode(specification.base.fillMode);
        rasterizer.cullMode = VkCullModeFromCullMode(specification.base.cullMode);
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;
        rasterizer.lineWidth = 1.0f;

        VkPipelineViewportStateCreateInfo viewportInfo = {};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = nullptr;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = nullptr;

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        
        VkRenderPass renderPass = m_device->GetRenderPassCache().Acquire(specification.renderPassKey).Get();

        std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
        {
            auto& subpass = specification.renderPassKey.subpasses[specification.subpass];
            auto size = subpass.colorAttachmentCount; 
            blendAttachments.resize(size);

            for (std::size_t i = 0, n = subpass.colorAttachmentCount; i < n; i++)
            {
                VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        
                colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
                colorBlendAttachment.blendEnable = VK_FALSE;
                
                // colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                // colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                // colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
                // colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                // colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                // colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

                blendAttachments[i] = colorBlendAttachment;
            }
        }

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = blendAttachments.size();
        colorBlending.pAttachments = blendAttachments.data();

        VkPipelineMultisampleStateCreateInfo multisample = {};
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.sampleShadingEnable = VK_FALSE;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.pVertexInputState = &vertexInput.vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pMultisampleState = &multisample;
        pipelineInfo.pDynamicState = &dynamicState;
        
        HL_EXCEPTION(vkCreateGraphicsPipelines(m_device->GetNativeDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS, "Unable to create VkPipeline");

        m_vertexShader = specification.base.vertexShader;
        m_pixelShader = specification.base.pixelShader;
        m_geometryShader = specification.base.geometryShader;
    }
    VkGraphicsPipeline::~VkGraphicsPipeline() 
    {        
        if (m_pipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(m_device->GetNativeDevice(), m_pipelineLayout, nullptr);
        }
        
        if (m_pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(m_device->GetNativeDevice(), m_pipeline, nullptr);
        }
    }
} // namespace Helios