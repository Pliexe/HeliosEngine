#include "BindingLayout.h"
#include "Helios/Core/Exceptions.h"
#include "Helios/Graphics/Graphics.h"

namespace Helios
{
    
    uint64_t BindingLayout::HashLayout(const BindingLayout &layout) {
        uint64_t hash = 14695981039346656037ULL; // FNV offset basis
        
        for (const auto &binding : layout.m_bindings) {
            hash ^= std::hash<uint32_t>{}(binding.slot);
            hash *= 1099511628211ULL;
            
            hash ^= std::hash<uint8_t>{}(static_cast<uint8_t>(binding.type));
            hash *= 1099511628211ULL;
            
            hash ^= std::hash<uint32_t>{}(binding.count);
            hash *= 1099511628211ULL;
            
            // hash ^= std::hash<uint32_t>{}(binding.flags);
            // hash *= 1099511628211ULL;
        }
        
        return hash;
    }
} // namespace Helios