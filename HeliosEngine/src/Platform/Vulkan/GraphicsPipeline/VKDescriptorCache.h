#pragma once

#include "Helios/Core/Exceptions.h"
#include "Helios/Core/Time.h"
#include "Helios/Core/UUID.h"
#include "Helios/Graphics/BindingLayout.h"
#include "Helios/Graphics/Graphics.h"
#include "Helios/Graphics/Sampler.h"
#include "Helios/Resources/Buffer.h"
#include "Helios/Resources/Texture.h"


#include "Platform/Vulkan/Sync/VkSyncableResource.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include "Platform/Vulkan/Buffers/VkUniformBuffer.h"
#include <vulkan/vulkan_core.h>


namespace Helios::Internal {
    struct ResourceBindingMap;
    struct ResourceBinding
    {
        using Resources = std::variant<
            std::vector<WeakRef<Image>>,
            std::vector<WeakRef<Sampler>>,
            std::vector<WeakRef<UnsafeUniformBuffer>>
        >;
        Resources resources;

        bool IsValid() const {
            return std::visit([](auto&& vec) {
                for (auto& res : vec)
                    if (res.expired())
                        return false;
                return true;
            }, resources);
        }

        bool operator==(const ResourceBinding& other) const {
            return std::visit([&](auto&& lhsVec, auto&& rhsVec) -> bool {
                using L = std::decay_t<decltype(lhsVec)>;
                using R = std::decay_t<decltype(rhsVec)>;
                
                if constexpr (!std::is_same_v<L, R>) {
                    return false;
                } else {
                    if (lhsVec.size() != rhsVec.size())
                        return false;

                    for (size_t i = 0; i < lhsVec.size(); ++i) {
                        auto lLock = lhsVec[i].lock();
                        auto rLock = rhsVec[i].lock();
                        if (!lLock || !rLock) return false;

                        using T = typename L::value_type::element_type;

                        if constexpr (std::is_same_v<T, Image>) {
                            if (lLock.get() != rLock.get()) return false;
                        } else {
                            if (lLock.get() != rLock.get()) return false;
                        }
                    }
                    return true;
                }
            }, resources, other.resources);
        }


        std::size_t GetHash() const {
            return std::visit([](auto&& vec) {
                size_t hash = 0;
                for (auto& res : vec) {
                    auto locked = res.lock();
                    size_t h = 0;
                    if (locked) {
                        using T = typename std::decay_t<decltype(locked)>::element_type;
                        if constexpr (std::is_same_v<T, Image>) {
                            h = std::hash<void*>{}(locked.get());
                        } else {
                            h = std::hash<void*>{}(locked.get());
                        }
                    }
                    hash ^= h + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
                return hash;
            }, resources);
        }

        friend struct ResourceBindingMap;
        friend void WriteBindings(VkDescriptorSet descriptorSet, const BindingLayout& layout, const ResourceBindingMap& bindingMap);
    };

    struct ResourceBindingMap
    {
        std::unordered_map<BindingType, std::unordered_map<std::uint32_t, ResourceBinding>> bindings;

        bool IsValid() {
            for (auto& [slot, bindings] : bindings)
                for (auto& [_, binding] : bindings)
                    if (!binding.IsValid()) return false;
            return true;
        }

        template <class T>
        void SetResource(uint32_t slot, T resource) {
            static_assert(
                std::is_same_v<T, Ref<Image>> ||
                std::is_same_v<T, Ref<Sampler>> ||
                std::is_same_v<T, Ref<UnsafeUniformBuffer>>,
                "Invalid resource type for SetResource"
            );
            BindingType type;
            if constexpr (std::is_same_v<T, Ref<Image>>)
                bindings[BindingType::Image][slot] = { std::vector<WeakRef<Image>>{ resource } };
            else if constexpr (std::is_same_v<T, Ref<Sampler>>)
                bindings[BindingType::Sampler][slot] = { std::vector<WeakRef<Sampler>>{ resource } };
            else if constexpr (std::is_same_v<T, Ref<UnsafeUniformBuffer>>)
                bindings[BindingType::UniformBuffer][slot] = { std::vector<WeakRef<UnsafeUniformBuffer>>{ resource } };
        }

        template <class T>
        void SetResource(uint32_t slot, const std::vector<T>& resources) {
            static_assert(
                std::is_same_v<T, Ref<Image>> ||
                std::is_same_v<T, Ref<Sampler>> ||
                std::is_same_v<T, Ref<UnsafeUniformBuffer>>,
                "Invalid resource type for SetResource"
            );
            if constexpr (std::is_same_v<T, Ref<Image>>)
            {
                std::vector<WeakRef<Image>> imgResources;
                imgResources.reserve(resources.size());
                for (auto& tex : resources)
                    imgResources.push_back(tex);
                bindings[BindingType::Image][slot] = { .resources = imgResources };
            }
            else
            {
                BindingType type;
                if constexpr (std::is_same_v<T, Ref<Image>>)
                    type = BindingType::Image;
                else if constexpr (std::is_same_v<T, Ref<Sampler>>)
                    type = BindingType::Sampler;
                else if constexpr (std::is_same_v<T, Ref<UnsafeUniformBuffer>>)
                    type = BindingType::UniformBuffer;

                bindings[type][slot] = { .resources = resources };
            }
        }
        
        bool operator==(const ResourceBindingMap& other) const {
            return bindings == other.bindings;
        }

        std::size_t GetHash() const {
            size_t hash = 0;
            for (const auto& [type, typeMap] : bindings) {
                size_t typeHash = std::hash<uint32_t>()(static_cast<uint32_t>(type));
                for (const auto& [slot, binding] : typeMap) {
                    size_t slotHash = std::hash<uint32_t>()(slot);
                    size_t bindingHash = binding.GetHash();

                    hash ^= typeHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                    hash ^= slotHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                    hash ^= bindingHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
                }
            }
            return hash;
        }
    };
    
    struct VkDescriptorSetKey
    {
        BindingLayout layout; // Used for filling missing resource with dummies
        ResourceBindingMap bindingMap;

        bool operator==(const VkDescriptorSetKey& other) const
        {
            return layout == other.layout && bindingMap == other.bindingMap;
        }
    };

    struct VkDescriptorSetKeyHasher
    {
        size_t operator()(const VkDescriptorSetKey& descSetKey) const noexcept {
            size_t h1 = descSetKey.layout.GetHash();
            size_t h2 = descSetKey.bindingMap.GetHash();
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
    
    struct DescriptorSetCachedItem
    {
        Ref<VkDescriptorSet> descriptorSet;
        // std::unordered_set<VkFence> 
        unsigned long long lastUsage;
    };
    
    class VkDescriptorSetCache
    {
    public:
        void Init(const Ref<VkGraphicsDevice>& device, uint32_t maxStaticSets = 1000, uint32_t maxTransientSetsPerFrame = 128);
        void Shutdown();
    
        // Ref<VkDescriptorSet> GetDescriptorSet(const VkDescriptorSetKey &key, VkDescriptorSetLayout &vklayout, bool transient); // TODO: Remove later
        
        Ref<VkDescriptorSet> Acquire(const VkDescriptorSetKey& key, VkDescriptorSetLayout& vklayout);
        
        void Prepare();

        void UpdateFrameTicket(SyncTicket ticket) { m_tickets[static_cast<std::size_t>(Graphics::GetCurrentFrameIndex())] = ticket; }

        std::size_t GetStaticCacheSize() const { return m_staticDescriptorSetCache.size(); }
        std::size_t GetTransientCacheSize() const { return m_transientDescriptorSetCache.size(); }
        
    private:
        std::unordered_map<VkDescriptorSetKey, DescriptorSetCachedItem, VkDescriptorSetKeyHasher> m_staticDescriptorSetCache;
        std::unordered_map<VkDescriptorSetKey, std::pair<std::uint64_t, DescriptorSetCachedItem>, VkDescriptorSetKeyHasher> m_transientDescriptorSetCache;

        VkDescriptorPool m_staticDescriptorPool;
        std::vector<VkDescriptorPool> m_transientDescriptorPools;
        std::vector<SyncTicket> m_tickets;

        Ref<VkGraphicsDevice> m_device;
    };
}