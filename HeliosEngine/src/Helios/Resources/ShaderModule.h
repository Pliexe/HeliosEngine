#pragma once

#include "Helios/Core/Logger.h"
#include "Helios/Core/UUID.h"
#include "pch.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Core/Base.h"
#include "Helios/Core/Bitmask.h"

#include "Buffer.h"
#include <filesystem>
#include <initializer_list>
#include <optional>
#include <vector>

#include "xxh3.h"

namespace Helios
{
    class ShaderModule
    {
    public:
    
        enum class Family
        {
            Main,
            Surface,
            Compute,
            Function,
        };
    
        // Only possible to combine if family type is function
        enum class Stage : bitmask_t
        {
            None     = BIT(0),
            Vertex   = BIT(1),
            Geometry = BIT(2),
            Pixel    = BIT(3),
            Compute  = BIT(4),
        };     


        enum class StorageType
        {
            Code,
            IR
        };

        struct ShaderModuleSpecification
        {
            Family family;
            Stage stage;
            StorageType storageType;
            std::string tag;
            
            std::filesystem::path path;
            std::initializer_list<std::string> definitions;
            std::optional<InputLayouts> inputLayout;
        };
        
        ShaderModule(ShaderModuleSpecification moduleSpecification);
        // IR CODE
        // Intermidiate Representation
        // ShaderModule(IR, ModuleType type);

        Family GetFamily() const { return m_family; }
        Stage GetStage() const { return m_stage; }
        std::string_view GetTag() const { return m_tag; }
        const std::vector<std::string>& GetDefinitions() const { return m_definitions; }
        const UUID GetUUID() const { return m_uuid; }
        const std::filesystem::path& GetPath() const { return m_shaderPath; }
        const std::optional<InputLayouts>& GetInputLayouts() const { return m_inputLayout; }

        const XXH128_hash_t GetHash() const { return m_hash; }
        
        bool operator==(const ShaderModule& rhs) const { return rhs.m_hash.low64 == m_hash.low64 && rhs.m_hash.high64 == m_hash.high64; }
        bool operator<(const ShaderModule& rhs) const { return m_hash.high64 < rhs.m_hash.high64 || (m_hash.high64 == rhs.m_hash.high64 && m_hash.low64 < rhs.m_hash.low64); }

    private:
        UUID m_uuid;
        Family m_family;
        Stage m_stage;
        StorageType m_storageType;
        std::vector<std::string> m_definitions;
        std::string m_tag;
        std::filesystem::path m_shaderPath;
        std::optional<InputLayouts> m_inputLayout;

        XXH128_hash_t m_hash;

    };

    // inline ShaderModule::Stage operator|(ShaderModule::Stage lhs, ShaderModule::Stage rhs)
    // {
    //     return static_cast<ShaderModule::Stage>(
    //         static_cast<std::underlying_type_t<ShaderModule::Stage>>(lhs) |
    //         static_cast<std::underlying_type_t<ShaderModule::Stage>>(rhs));
    // }

    // inline ShaderModule::Stage operator&(ShaderModule::Stage lhs, ShaderModule::Stage rhs)
    // {
    //     return static_cast<ShaderModule::Stage>(
    //         static_cast<std::underlying_type_t<ShaderModule::Stage>>(lhs) &
    //         static_cast<std::underlying_type_t<ShaderModule::Stage>>(rhs));
    // }

    ENABLE_BITMASK_OPERATORS(ShaderModule::Stage);
}
