#pragma once

#include "Helios/Core/Bitmask.h"
#include "Helios/Core/Exceptions.h"
#include "pch.h"
#include "Helios/Core/Base.h"
#include <string_view>
#include <unordered_map>

namespace Helios
{
    
    enum class BindingType
    {
        Unknown,
        UniformBuffer,
        Image,
        Sampler,
        CombinedTextureSampler2D,
    };

    inline std::string to_string(BindingType type)
    {
        switch (type)
        {
        case BindingType::Unknown: return "Unknown";
        case BindingType::UniformBuffer: return "UniformBuffer";
        case BindingType::Image: return "Image";
        case BindingType::Sampler: return "Sampler";
        case BindingType::CombinedTextureSampler2D: return "CombinedTextureSampler2D";
        }

        return "Unknown";
    }
    
    struct BindingInfo
    {
        std::uint32_t slot;
        BindingType type;
        uint32_t count = 1;
    };

    inline bool operator==(const BindingInfo& a, const BindingInfo& b)
    {
        return a.slot == b.slot &&
            a.type == b.type &&
            a.count == b.count;
    }

    struct BindingInfoHasher
    {
        std::size_t operator()(const BindingInfo& b) const noexcept
        {
            std::size_t h = std::hash<uint32_t>{}(b.slot);
            h ^= std::hash<int>{}(static_cast<int>(b.type)) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<uint32_t>{}(b.count) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
    
    struct BindingLayout
    {
        BindingLayout() = delete;

        BindingLayout(std::initializer_list<BindingInfo> bindings)
        : m_bindings(NormalizeAndSortBindings(bindings)),
        m_cachedHash(ComputeHash()) {}

        BindingLayout(const std::vector<BindingInfo>& bindings)
        : m_bindings(NormalizeAndSortBindings(bindings)),
        m_cachedHash(ComputeHash()) {}

        BindingLayout(const BindingLayout& other) = default;
        
        uint64_t HashLayout(const BindingLayout &layout);
        
        std::vector<BindingInfo> m_bindings;
        
        bool operator==(const BindingLayout& other) const
        {
            if (m_cachedHash != other.m_cachedHash) return false;
            if (m_bindings.size() != other.m_bindings.size()) return false;
            
            for (size_t i = 0; i < m_bindings.size(); ++i)
                if (!(m_bindings[i] == other.m_bindings[i])) return false;
            
            return true;
        }

        const size_t GetHash() const { return m_cachedHash; }
        
    private:
        size_t m_cachedHash;
        size_t ComputeHash() const
        {
            size_t seed = 0;
            BindingInfoHasher infoHasher;
            for (const auto& binding : m_bindings)
            {
                size_t h = infoHasher(binding);
                seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }

        static bool BindingInfoComparator(const BindingInfo& a, const BindingInfo& b)
        {
            if (a.slot != b.slot) return a.slot < b.slot;
            if (a.type != b.type) return static_cast<int>(a.type) < static_cast<int>(b.type);
            if (a.count != b.count) return a.count < b.count;
            return true;
        }
        
        static std::vector<BindingInfo> NormalizeAndSortBindings(std::vector<BindingInfo> bindings)
        {
            std::sort(bindings.begin(), bindings.end(), BindingInfoComparator);
            return bindings;
        }
    };

    struct BindingLayoutHasher
    {
        size_t operator()(const BindingLayout& layout) const noexcept
        {
            return layout.GetHash();
        }
    };

    class BindingLink
    {
    public:
        using Set = uint32;
        using Slot = uint32;
        using Name = std::string;
        
        struct Binding
        {
            BindingType type;
            Set set;
            Slot slot;
        };

        void SetBinding(const Name& name, const BindingType& type, const Set& set, const Slot& slot) { m_bindings[name] = { type, set, slot }; }
        
        bool Exists(const Name& name) const { return m_bindings.find(name) != m_bindings.end(); }
        const Binding* GetBinding(const std::string_view& name) const { return m_bindings.find(name.data()) != m_bindings.end() ? &m_bindings.at(name.data()) : nullptr; }
        const Binding* GetBinding(const Name& name) const { return m_bindings.find(name) != m_bindings.end() ? &m_bindings.at(name) : nullptr; }

        const std::unordered_map<Name, Binding>& GetBindings() const { return m_bindings; }
    private:

        std::unordered_map<Name, Binding> m_bindings;
    };
}