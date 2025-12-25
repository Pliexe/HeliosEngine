#pragma once

#include "Helios/Core/Hash.h"
#include "Helios/Graphics/BindingLayout.h"
#include "pch.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/ShaderModule.h"
#include "Helios/Core/UUID.h"
#include <optional>
#include <unordered_map>

namespace Helios
{
    struct ShaderKey {
        ShaderModule::Stage stage;
        uint64_t combinedHash;
        std::vector<XXH128_hash_t> moduleHashes; // order matters

        ShaderKey(ShaderModule::Stage stage, const std::vector<Ref<ShaderModule>>& modules) : stage(stage)
        {
            moduleHashes.reserve(modules.size());
            for (auto& mod : modules) moduleHashes.push_back(mod->GetHash());

            combinedHash = XXH3_64bits(moduleHashes.data(), moduleHashes.size() * sizeof(XXH128_hash_t));
        }

        bool operator==(const ShaderKey& other) const {
            if (stage != other.stage) return false;
            if (combinedHash != other.combinedHash) return false;
            if (moduleHashes.size() != other.moduleHashes.size()) return false;

            for (std::size_t i=0,n=moduleHashes.size(); i<n; i++)
                if (moduleHashes[i].low64 != other.moduleHashes[i].low64 || moduleHashes[i].high64 != other.moduleHashes[i].high64) return false;
            
            return true;
        }
    };

    
    class Shader
    {       
    public:
        virtual ~Shader() = default;
    
        enum class Stage
        {
            Vertex,
            Pixel,
            Geometry,
            Compute,
        };
    
        const UUID& GetUUID() const { return m_uuid; }
        const Stage& GetStage() const { return m_stage; }
        constexpr std::string_view GetDispalyName() const { return m_displayName; }

        const std::vector<BindingLayout>& GetBindingLayouts() const { return m_bindingLayouts; }
        using SetSlot = std::uint32_t;
        const std::unordered_map<Hash128Key, SetSlot, Hash128KeyHasher, Hash128KeyEqual>& GetModuleToSlotMap() { return m_bindingMap; }

        const BindingLink& GetBindingLink() const { return m_bindingLink; }
        const std::optional<ShaderKey>& GetKey() const { return m_key; }

    protected:
        std::string m_displayName;
        std::vector<BindingLayout> m_bindingLayouts;
        std::unordered_map<Hash128Key, SetSlot, Hash128KeyHasher, Hash128KeyEqual> m_bindingMap;
        UUID m_uuid;
        Stage m_stage;
        std::optional<ShaderKey> m_key;
        BindingLink m_bindingLink;

        friend class ResourceResolver;
    };

    class VertexShader : public Shader
    {
    public:
		virtual const InputLayouts& GetInputLayouts() const = 0;

        static Ref<VertexShader> Create(const std::vector<Ref<ShaderModule>>& shaderModules);
        static Ref<VertexShader> Create(const std::filesystem::path &path, InputLayouts inputLayouts);

        static void Validate(const std::vector<Ref<ShaderModule>>& shaderModules);
    };

    class PixelShader : public Shader
    {
    public:
        static Ref<PixelShader> Create(const std::vector<Ref<ShaderModule>>& shaderModules);
        static Ref<PixelShader> Create(const std::filesystem::path& path);        

        static void Validate(const std::vector<Ref<ShaderModule>>& shaderModules);
    };

    class GeometryShader : public Shader
    {
    public:
        static Ref<GeometryShader> Create(const std::vector<Ref<ShaderModule>>& shaderModules);
        static Ref<GeometryShader> Create(const std::filesystem::path& path);        

        static void Validate(const std::vector<Ref<ShaderModule>>& shaderModules);
    };
    
    class ComputeShader : public Shader
    {
    public:
        static Ref<ComputeShader> Create(const std::vector<Ref<ShaderModule>>& shaderModules);
        static Ref<ComputeShader> Create(const std::filesystem::path& path);        

        static void Validate(const std::vector<Ref<ShaderModule>>& shaderModules);
    };
}


namespace std
{
	template <>
    struct hash<Helios::ShaderKey> {
        size_t operator()(const Helios::ShaderKey& key) const {
            uint64_t h = key.combinedHash ^ (static_cast<uint64_t>(key.stage) << 32);
            return static_cast<size_t>(h);
        }
    };
}