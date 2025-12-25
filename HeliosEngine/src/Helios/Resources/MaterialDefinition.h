#pragma once

#include "Helios/Core/Base.h"
#include "Helios/Resources/ShaderModule.h"

namespace Helios
{
    class MaterialDefinition
    {
    public:
        
        enum class Family
        {
            Surface,
            Gizmos,
            PostProcess,
        };

        enum class BackCulling 
        {
            Front,
            Back,
            Both
        };

        enum class Features : bitmask_t
        {
            None = BIT(0),
        };

        enum class LightingModel
        {
            Unlit,
            Lit
        };


        struct MaterialDefinitionSpecification
        {
            Ref<ShaderModule> shaderModule;
            Family family;
            BackCulling backCulling;
            LightingModel lightingModel;
        };



    private:

        std::string m_name;

    };
}