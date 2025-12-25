#include "ShaderModule.h"
#include "Helios/Core/Exceptions.h"
#include <algorithm>

namespace Helios
{
    ShaderModule::ShaderModule(ShaderModuleSpecification spec)
    {
        bool isVertexMain = (spec.family == Family::Main) && any(spec.stage & Stage::Vertex);
        if (spec.inputLayout.has_value())
        {
            HL_EXCEPTION(!isVertexMain, "InputLayouts may only be defined on Stage::Vertex of Family::Main!");
        }
        else
        {
            HL_EXCEPTION(isVertexMain, "Missing InputLayouts from Stage::Vertex main module!");
        }


        // if (spec.storageType == StorageType::Code)
        // {
        //     HL_EXCEPTION(!spec.path.has_value(), "Missing path while StorageType Code was specified!");
        // }

        if (spec.storageType == StorageType::Code)
        {
            HL_EXCEPTION(spec.path.empty(), "Missing path while StorageType Code was specified!");
        }

        if (spec.storageType == StorageType::IR)
        {
            HL_EXCEPTION(true, "Intermidiate Representation is not supported yet!");
        }
        
        // m_bindingLayout = spec.bindingLayout;
        
        m_inputLayout = spec.inputLayout;
        m_family = spec.family;
        m_stage = spec.stage;
        m_storageType = spec.storageType;
        m_tag = spec.tag;
        m_shaderPath = spec.path;
        
        // m_shaderPath = spec.path.value();

        // switch (spec.family)
        // {
        //     case Family::Main:
        //     {

        //     }
        //     default:
        //         HL_EXCEPTION(true, "Family not supported yet!");
        // }


        XXH3_state_t* const state = XXH3_createState();
        XXH3_128bits_reset(state);

        auto pathStr = std::filesystem::absolute(m_shaderPath).string();
        XXH3_128bits_update(state, pathStr.data(), pathStr.size());

        XXH3_128bits_update(state, m_tag.data(), m_tag.size());
        
        XXH3_128bits_update(state, &m_stage, sizeof(m_stage));
        XXH3_128bits_update(state, &m_family, sizeof(m_family));

        XXH3_128bits_update(state, &m_storageType, sizeof(m_storageType));

        if (m_inputLayout.has_value())
        {
            const auto& layouts = m_inputLayout.value();
            
            for (auto& layout : layouts.GetLayouts())
            {
                uint32_t stride = layout.GetStride();
                InputClassification m_inputClassification = layout.GetInputClassification();
                
                XXH3_128bits_update(state, &stride, sizeof(stride));
                XXH3_128bits_update(state, &m_inputClassification, sizeof(m_inputClassification));

                for (auto& element : layout.GetElements())
                {
                    HL_EXCEPTION(element.name.empty(), "InputLayout Element name is not allowed to be unnamed!"); 
                    // Can't tell if I should include \0 null terminator. Oh well...
                    XXH3_128bits_update(state, element.name.data(), element.name.length() + 1);
                    XXH3_128bits_update(state, &element.type, sizeof(element.type));
                }
            }
        }

        // MIGRATED TO REFLECTION IN SHADER
        // if(m_bindingLayout.has_value())
        // {
        //     std::sort(m_bindingLayout.value().m_bindings.begin(), m_bindingLayout.value().m_bindings.end(), [](const BindingInfo& a, const BindingInfo& b) {
        //         return a.slot < b.slot;
        //     });

        //     for (auto& binding : m_bindingLayout.value().m_bindings)
        //     {
        //         XXH3_128bits_update(state, &binding.slot, sizeof(binding.slot));
        //         XXH3_128bits_update(state, &binding.type, sizeof(binding.type));
        //         XXH3_128bits_update(state, &binding.count, sizeof(binding.count));
        //     }
        // }

        if (spec.definitions.size() > 0)
        {
            for (auto& def : spec.definitions)
            {
                XXH3_128bits_update(state, def.data(), def.length() + 1);
                m_definitions.push_back(def);
            }
        }

        m_hash = XXH3_128bits_digest(state);
        XXH3_freeState(state);

    }
}