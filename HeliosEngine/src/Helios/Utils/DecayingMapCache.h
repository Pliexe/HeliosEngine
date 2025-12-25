
/* Copyright (c) 2025 Szabadi László Zsolt
* You should have received a copy of the GNU AGPL v3.0 license with
* this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
*/
#pragma once

#include "Helios/Core/Exceptions.h"
#include <cstdint>
#include <unordered_map> 
#include <chrono>

namespace Helios
{
    template <
        typename Key,
        typename Value,
        std::uint64_t Threshold,
        bool IsTickBased,
        typename Hasher = std::hash<Key>
    >
    class DecayingMapCache
    {
    private:
        struct CachedItem
        {
            Value item;
            std::uint64_t lastUsed;
            bool usedThisStep = false;
        };
    public:

        // DecayingCache(std::uint64_t threshold) requires(IsTickBased) : m_threshold(threshold) { }
        // DecayingCache(std::uint64_t time_ms_threshold) requires(!IsTickBased) : m_threshold(time_ms_threshold) { }
        
        bool Contains(Key key)
        {
            return m_cache.contains(key);
        }

        std::size_t Size() const 
        {
            return m_cache.size();
        }

        Value* Get(const Key& key)
        {
            auto it = m_cache.find(key);
            if (it != m_cache.end())
            {
                it->second.usedThisStep = true;
                return &it->second.item;
            }
            return nullptr;
        }

        Value& GetRef(const Key& key)
        {
            auto it = m_cache.find(key);
            if (it != m_cache.end())
            {
                it->second.usedThisStep = true;
                return it->second.item;
            }
            HL_EXCEPTION(true, "A Value with the given Key does not exist!");
        }

        bool Erase(Key key)
        {
            return m_cache.erase(key);
        }


        Value& GetOrCreate(const Key& key)
        {
            auto [it, inserted] = m_cache.try_emplace(key);
            it->second.usedThisStep = true;
            return it->second.item;
        }

        template <typename T>
        Value& Insert(const Key& key, T&& value)
        {
            auto lastUsed = 0u;
            if constexpr (!IsTickBased) {
                auto current_time = std::chrono::system_clock::now().time_since_epoch();
                lastUsed = std::chrono::duration_cast<std::chrono::microseconds>(current_time).count();
            }

            auto [iter, inserted] = m_cache.insert({ 
                key, 
                CachedItem { .item = std::forward<T>(value), .lastUsed = lastUsed, .usedThisStep = true }
            });

            return iter->second.item;
        }

        template <typename T>
        Value& InsertOrAssign(const Key& key, T&& value)
        {
            auto lastUsed = 0u;
            if constexpr (!IsTickBased) {
                auto current_time = std::chrono::system_clock::now().time_since_epoch();
                lastUsed = std::chrono::duration_cast<std::chrono::microseconds>(current_time).count();
            }

            auto [iter, inserted] = m_cache.insert_or_assign({ 
                key, 
                CachedItem { .item = std::forward<T>(value), .lastUsed = lastUsed, .usedThisStep = true }
            });

            return iter->second.item;
        }

        void Clear()
        {
            m_cache.clear();
        }
        
        void Step() requires(IsTickBased)
        {
            for (auto it = m_cache.begin(); it != m_cache.end(); ) {
                if (!it->second.usedThisStep && it->second.lastUsed >= Threshold) {
                    it = m_cache.erase(it);
                    continue;
                } else {
                    if (it->second.usedThisStep) 
                    {
                        it->second.lastUsed = 0u;
                        it->second.usedThisStep = false;
                    }
                    else it->second.lastUsed++;
                    ++it;
                }
            }
        }

        void Step() requires(!IsTickBased)
        {
            auto current_time = std::chrono::system_clock::now().time_since_epoch();
            auto current_time_us = std::chrono::duration_cast<std::chrono::microseconds>(current_time).count();
            
            for (auto it = m_cache.begin(); it != m_cache.end(); ) {
                if (!it->second.usedThisStep && current_time_us - it->second.lastUsed >= Threshold) {
                    it = m_cache.erase(it);
                    continue;
                } else {
                    if (it->second.usedThisStep)
                    {
                        it->second.lastUsed = current_time_us;
                        it->second.usedThisStep = false;
                    }
                    ++it;
                }
            }
        }

    private:      
        std::unordered_map<Key, CachedItem, Hasher> m_cache;
        // std::uint64_t m_threshold;
    };
}