
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
        typename Value,
        std::uint64_t Threshold,
        bool IsTickBased
    >
    class DecayingCache
    {
    private:
        struct CachedItem
        {
            Value item;
            std::uint64_t lastUsed;
            bool usedThisStep = false;
        };
    public:

        bool empty() const { return m_items.empty(); }

        std::size_t size() const { return m_items.size(); }

        Value& operator[](std::size_t index)
        {
            if (index >= m_items.size()) HL_EXCEPTION(true, "A Value with the given Index does not exist!");
            m_items[index].usedThisStep = true;
            return m_items[index].item;
        }

        bool erase(std::size_t index)
        {
            if (index >= m_items.size()) return false;
            m_items.erase(m_items.begin() + index);
            return true;
        }

        void push_back(Value&& value)
        {
            m_items.push_back({ .item = std::move(value), .lastUsed = 0u, .usedThisStep = true });
        }

        void push_back(const Value& value)
        {
            m_items.push_back({ .item = value, .lastUsed = 0u, .usedThisStep = true });
        }

        void clear()
        {
            m_items.clear();
        }

        Value pop_front()
        {
            HL_EXCEPTION(m_items.empty(), "The Cache is empty!");
            Value value = std::move(m_items.front().item);
            m_items.erase(m_items.begin());
            return value;
        }
        
        void step() requires(IsTickBased)
        {
            for (auto it = m_items.begin(); it != m_items.end(); ) {
                if (!it->usedThisStep && it->lastUsed >= Threshold) {
                    it = m_items.erase(it);
                    continue;
                } else {
                    if (it->usedThisStep) 
                    {
                        it->lastUsed = 0u;
                        it->usedThisStep = false;
                    }
                    else it->lastUsed++;
                    ++it;
                }
            }
        }

        void step() requires(!IsTickBased)
        {
            auto current_time = std::chrono::system_clock::now().time_since_epoch();
            auto current_time_us = std::chrono::duration_cast<std::chrono::microseconds>(current_time).count();
            
            for (auto it = m_items.begin(); it != m_items.end(); ) {
                if (!it->usedThisStep && current_time_us - it->lastUsed >= Threshold) {
                    it = m_items.erase(it);
                    continue;
                } else {
                    if (it->usedThisStep)
                    {
                        it->lastUsed = current_time_us;
                        it->usedThisStep = false;
                    }
                    ++it;
                }
            }
        }

    private:      
        std::vector<CachedItem> m_items;
    };
}