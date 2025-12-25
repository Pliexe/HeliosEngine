#pragma once

#include "Helios/Graphics/BindingLayout.h"
#include "Platform/Vulkan/VkWrappers.h"
#include "pch.h"
#include "Platform/Vulkan/VkGraphicsDevice.h"
#include <unordered_map>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    struct VkLayoutSetKey
    {
        std::optional<BindingLayout> vertex;
        std::optional<BindingLayout> pixel;
        std::optional<BindingLayout> geometry;

        bool operator==(const VkLayoutSetKey& other) const
        {
            return vertex == other.vertex && pixel == other.pixel && geometry == other.geometry;
        }
    };

    struct VkLayoutSetKeyHasher
    {
        size_t operator()(const VkLayoutSetKey& key) const noexcept
        {
            size_t seed = 0;
            auto hashCombine = [](size_t& seed, size_t val) {
                seed ^= val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            };

            if (key.vertex.has_value())
                hashCombine(seed, key.vertex->GetHash());
            if (key.pixel.has_value())
                hashCombine(seed, key.pixel->GetHash());
            if (key.geometry.has_value())
                hashCombine(seed, key.geometry->GetHash());

            return seed;
        }
    };

    inline VkDescriptorType GetDescType(BindingType type)
    {
        switch (type)
        {
        case BindingType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case BindingType::Image: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case BindingType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case BindingType::CombinedTextureSampler2D: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default: break;
        }
        HL_EXCEPTION(true, "Unknown binding type!");
    }
    
    class VkDescriptorSetLayoutCache
    {
    public:

        VkDescriptorSetLayoutCache(const Ref<VkGraphicsDevice>& device) : m_device(device) { }    

        Ref<Vulkan::VkDescriptorSetLayoutWrapper> Acquire(const VkLayoutSetKey& key)
        {
            auto it = m_cache.find(key);
            if (it != m_cache.end())
            {
                if (auto cached = it->second.lock())
                    return cached;
            }
            
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 0;
            createInfo.pBindings = nullptr;

            
            std::vector<VkDescriptorSetLayoutBinding> vkBindings;
            using SlotBindings = std::unordered_map<VkDescriptorType, VkDescriptorSetLayoutBinding>;
            std::unordered_map<std::uint32_t, SlotBindings> bindings;

            auto addToBindings = [&](const BindingInfo& info, VkShaderStageFlagBits flagsToAdd)
            {
                VkDescriptorSetLayoutBinding vkBinding{};
                vkBinding.binding = info.slot;
                vkBinding.descriptorType = GetDescType(info.type);
                vkBinding.descriptorCount = info.count;
                vkBinding.stageFlags = flagsToAdd;
                vkBinding.pImmutableSamplers = nullptr;
            
                auto [val, res] = bindings[info.slot].try_emplace(vkBinding.descriptorType, vkBinding);
                if (!res)
                {
                    val->second.stageFlags |= flagsToAdd;
                }
            };
            
            if (key.vertex.has_value()) for (const auto& binding : key.vertex->m_bindings) addToBindings(binding, VK_SHADER_STAGE_VERTEX_BIT);
            if (key.pixel.has_value()) for (const auto& binding : key.pixel->m_bindings) addToBindings(binding, VK_SHADER_STAGE_FRAGMENT_BIT);
            if (key.geometry.has_value()) for (const auto& binding : key.geometry->m_bindings) addToBindings(binding, VK_SHADER_STAGE_GEOMETRY_BIT);

            for (auto& binds : bindings)
                for (auto& bindPerType : binds.second)
                    vkBindings.push_back(bindPerType.second);
            
            createInfo.bindingCount = static_cast<uint32_t>(vkBindings.size());
            createInfo.pBindings = vkBindings.data();

            auto result = Vulkan::VkDescriptorSetLayoutWrapper::Create(*m_device, &createInfo, nullptr);
            HL_EXCEPTION(!result.has_value(), "Failed to create descriptor set layout");

            auto ref = CreateRef<Vulkan::VkDescriptorSetLayoutWrapper>(std::move(result.value()));

            m_cache[key] = ref;
            return ref;
        }


        void Step()
        {
            
        }
        
    private:

        Ref<VkGraphicsDevice> m_device;
        std::unordered_map<VkLayoutSetKey, std::weak_ptr<Vulkan::VkDescriptorSetLayoutWrapper>, VkLayoutSetKeyHasher> m_cache;
    };
}