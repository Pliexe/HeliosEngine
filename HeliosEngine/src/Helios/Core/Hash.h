// Wrapper designed for storing 128bit values in maps
#pragma once
#include "pch.h"

namespace Helios
{
    using Hash128Key = std::pair<std::uint64_t, std::uint64_t>;
    struct Hash128KeyHasher 
    {
        size_t operator()(const Hash128Key& k) const noexcept {
            size_t h1 = std::hash<uint64_t>{}(k.first);
            size_t h2 = std::hash<uint64_t>{}(k.second);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };

    struct Hash128KeyEqual
    {
        bool operator()(const Hash128Key& a, const Hash128Key& b) const noexcept {
            return a.first == b.first && a.second == b.second;
        }
    };
}