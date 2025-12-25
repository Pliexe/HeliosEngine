#include "VKDescriptorCache.h"
#include "Helios/Core/Exceptions.h"
#include "Platform/Vulkan/Buffers/VkUniformBuffer.h"
#include "Platform/Vulkan/Sync/VkSyncableResource.h"
#include "Platform/Vulkan/VkImage.h"
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Helios::Internal
{
    void VkDescriptorSetCache::Init(const Ref<VkGraphicsDevice>& device, uint32_t maxStaticSets, uint32_t maxTransientSetsPerFrame)
    {
        m_device = device;
        {
            std::array<VkDescriptorPoolSize, 4> poolSizes{};
            poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxStaticSets };
            poolSizes[1] = { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxStaticSets };
            poolSizes[2] = { VK_DESCRIPTOR_TYPE_SAMPLER, maxStaticSets };
            poolSizes[3] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxStaticSets };

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = maxStaticSets;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();

            HL_EXCEPTION(vkCreateDescriptorPool(device->GetNativeDevice(), &poolInfo, nullptr, &m_staticDescriptorPool) != VK_SUCCESS, "Unable to create static descriptor pool!");
        }

        m_transientDescriptorPools.resize(Graphics::GetMaxFramesInFlight());
        m_tickets.resize(Graphics::GetMaxFramesInFlight());
        for (uint32_t i = 0; i < Graphics::GetMaxFramesInFlight(); ++i)
        {
            std::array<VkDescriptorPoolSize, 4> poolSizes{};
            poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxTransientSetsPerFrame };
            poolSizes[1] = { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxTransientSetsPerFrame };
            poolSizes[2] = { VK_DESCRIPTOR_TYPE_SAMPLER, maxTransientSetsPerFrame };
            poolSizes[3] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxTransientSetsPerFrame };

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = maxTransientSetsPerFrame;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();

            VkDescriptorPool pool = VK_NULL_HANDLE;
            HL_EXCEPTION(vkCreateDescriptorPool(device->GetNativeDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS, "Unable to create dynamic descriptor pool!");

            m_transientDescriptorPools[i] = pool;
        }
    }

    void VkDescriptorSetCache::Shutdown()
    {
        m_staticDescriptorSetCache.clear();
        m_transientDescriptorSetCache.clear();

        if (m_staticDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(m_device->GetNativeDevice(), m_staticDescriptorPool, nullptr);
            m_staticDescriptorPool = VK_NULL_HANDLE;
        }

        for (auto& pool : m_transientDescriptorPools)
        {
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(m_device->GetNativeDevice(), pool, nullptr);
            }
        }

        m_transientDescriptorPools.clear();
    }

    VkDescriptorSet AllocateDescriptorSet(VkDevice device, VkDescriptorPool pool, VkDescriptorSetLayout vklayout)
    {
        VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &vklayout;

        VkDescriptorSet descriptorSet;
        HL_EXCEPTION (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS, "Failed to allocate descriptor set.");

        return descriptorSet;
    }

    VkDescriptorBufferInfo CreateBufferInfo(const Ref<VkUniformBuffer>& ub)
    {
        VkDescriptorBufferInfo info{};
        info.buffer = ub->GetBuffer();
        info.offset = 0;
        info.range = ub->GetSize();
        return info;
    }

    VkDescriptorImageInfo CreateCombinedImageSamplerInfo(const Ref<Image>& img, const Ref<VkSampler>& samp)
    {
        VkDescriptorImageInfo info{};
        info.imageView = std::static_pointer_cast<VkImage>(img)->GetViewImage();
        info.sampler = samp->GetNative();
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        return info;
    }

    VkDescriptorImageInfo CreateImageInfo(const Ref<Image>& img)
    {
        VkDescriptorImageInfo info{};
        info.imageView = std::static_pointer_cast<VkImage>(img)->GetViewImage();
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        std::cout << "CreateImageInfo: image=" << img 
                  << " view=" << info.imageView << std::endl;
        
        return info;
    }

    VkDescriptorImageInfo CreateImageInfo(const Ref<Texture2D>& tex)
    {
        return CreateImageInfo(tex->GetImage());
    }

    VkDescriptorImageInfo CreateSamplerInfo(const Ref<VkSampler>& samp)
    {
        VkDescriptorImageInfo info{};
        info.sampler = samp->GetNative();
        return info;
    }

    void WriteBindings(const Ref<VkGraphicsDevice>& device, VkDescriptorSet descriptorSet, const BindingLayout& bindingLayout, const ResourceBindingMap& bindingMap)
    {
        std::vector<VkWriteDescriptorSet> writes;
        writes.reserve(bindingLayout.m_bindings.size());
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorImageInfo> samplerInfos;
        std::vector<VkDescriptorImageInfo> combinedImageSamplerInfos;

        auto dummyUb = std::static_pointer_cast<VkUniformBuffer>(device->GetDummyUniformBuffer());
        assert(dummyUb);
        auto dummyUniformBufferInfo = CreateBufferInfo(dummyUb);

        auto dummyTex = device->GetDummyTexture2D();
        assert(dummyTex);
        auto dummyImage = std::static_pointer_cast<VkImage>(dummyTex->GetImage());

        auto dummyImageInfo = CreateImageInfo(dummyTex);

        auto dummySamp = std::static_pointer_cast<VkSampler>(device->GetDummySampler());
        assert(dummySamp);
        auto dummySamplerInfo = CreateSamplerInfo(dummySamp);

        auto dummyImageSamplerInfo = CreateCombinedImageSamplerInfo(dummyTex->GetImage(), dummySamp);

        for (std::size_t i = 0, n = bindingLayout.m_bindings.size(); i < n; ++i)
        {
            auto& binding = bindingLayout.m_bindings[i];

            auto typeIt = bindingMap.bindings.find(binding.type);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSet;
            write.dstBinding = binding.slot;
            write.dstArrayElement = 0;
            write.descriptorCount = binding.count;

            bool typeExists = typeIt != bindingMap.bindings.end();

            switch (binding.type) {
            case BindingType::UniformBuffer:
            {
                bufferInfos.reserve(bufferInfos.size() + binding.count);

                std::size_t existing_n = 0;
                auto bindIt = typeExists ? typeIt->second.find(binding.slot) : typeIt->second.end();
                if (bindIt != typeIt->second.end())
                {
                    std::visit([&](auto&& vec){
                        using T = std::decay_t<decltype(vec)>;
                        if constexpr (std::is_same_v<T, std::vector<WeakRef<UnsafeUniformBuffer>>>)
                        {
                            existing_n = std::min(static_cast<std::uint32_t>(vec.size()), binding.count);
                            if (existing_n > 0)
                            {
                                for (auto& ubPureWeak : vec)
                                {
                                    auto ubPure = ubPureWeak.lock();
                                    if (ubPure)
                                    {
                                        auto ub = std::static_pointer_cast<VkUniformBuffer>(ubPure);
                                        bufferInfos.emplace_back(CreateBufferInfo(ub));
                                    } else bufferInfos.push_back(dummyUniformBufferInfo);
                                }
                            }
                        }
                    }, bindIt->second.resources);
                }

                for (std::size_t i = existing_n, n = binding.count; i < n; ++i)
                    bufferInfos.push_back(dummyUniformBufferInfo);

                write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo = bufferInfos.data() + (bufferInfos.size() - binding.count);
                break;
            }
            case BindingType::CombinedTextureSampler2D:
            {
                combinedImageSamplerInfos.reserve(combinedImageSamplerInfos.size() + binding.count);

                std::vector<WeakRef<Image>> texVec;
                std::vector<WeakRef<Sampler>> sampVec;

                {
                    auto texIt = bindingMap.bindings.find(BindingType::Image);
                    if(texIt != bindingMap.bindings.end())
                        if(auto slotIt = texIt->second.find(binding.slot); slotIt != texIt->second.end())
                            std::visit([&](auto&& vec){ 
                                using T = std::decay_t<decltype(vec)>;
                                if constexpr (std::is_same_v<T, std::vector<WeakRef<Image>>>)
                                {
                                    texVec = vec; 
                                }
                            }, slotIt->second.resources);

                    auto sampIt = bindingMap.bindings.find(BindingType::Sampler);
                    if(sampIt != bindingMap.bindings.end())
                        if(auto slotIt = sampIt->second.find(binding.slot); slotIt != sampIt->second.end())
                            std::visit([&](auto&& vec){
                                using T = std::decay_t<decltype(vec)>;
                                if constexpr (std::is_same_v<T, std::vector<WeakRef<Sampler>>>)
                                {
                                    sampVec = vec;
                                }
                            }, slotIt->second.resources);
                }

                for (std::size_t i = 0, n = binding.count; i < n; ++i)
                {
                    Ref<Image> tex = (i < texVec.size()) ? texVec[i].lock() : nullptr;
                    Ref<Sampler>  samp = (i < sampVec.size()) ? sampVec[i].lock() : nullptr;

                    if (!tex && !samp) {
                        combinedImageSamplerInfos.push_back(dummyImageSamplerInfo);
                        continue;
                    }
                    if (!tex) tex = dummyTex->GetImage();
                    if (!samp) samp = dummySamp;
                    
                    combinedImageSamplerInfos.emplace_back(CreateCombinedImageSamplerInfo(
                        tex,
                        std::static_pointer_cast<VkSampler>(samp)
                    ));
                }

                write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo = combinedImageSamplerInfos.data() + (combinedImageSamplerInfos.size() - binding.count);
                break;
            }
            case BindingType::Image:
            {
                imageInfos.reserve(imageInfos.size() + binding.count);

                std::size_t existing_n = 0;
                auto bindIt = typeExists ? typeIt->second.find(binding.slot) : typeIt->second.end();
                if (bindIt != typeIt->second.end())
                {
                    std::visit([&](auto&& vec){
                        using T = std::decay_t<decltype(vec)>;
                        if constexpr (std::is_same_v<T, std::vector<WeakRef<Image>>>)
                        {
                            existing_n = std::min(static_cast<std::uint32_t>(vec.size()), binding.count);
                            if (existing_n > 0)
                            {
                                for (auto& texPureWeak : vec)
                                {
                                    auto texPure = texPureWeak.lock();
                                    if(texPure)
                                    {
                                        auto tex = texPure;
                                        imageInfos.emplace_back(CreateImageInfo(tex));
                                    } else imageInfos.push_back(dummyImageInfo);
                                }
                            }
                        }
                    }, bindIt->second.resources);
                }

                for (std::size_t i = existing_n, n = binding.count; i < n; ++i)
                    imageInfos.push_back(dummyImageInfo);

                write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                write.pImageInfo = imageInfos.data() + (imageInfos.size() - binding.count);
                break;
            }
            case BindingType::Sampler:
            {
                samplerInfos.reserve(samplerInfos.size() + binding.count);

                std::size_t existing_n = 0;
                auto bindIt = typeExists ? typeIt->second.find(binding.slot) : typeIt->second.end();
                if (bindIt != typeIt->second.end())
                {
                    std::visit([&](auto&& vec){
                        using T = std::decay_t<decltype(vec)>;
                        if constexpr (std::is_same_v<T, std::vector<WeakRef<Sampler>>>)
                        {
                            existing_n = std::min(static_cast<std::uint32_t>(vec.size()), binding.count);
                            if (existing_n > 0)
                            {
                                for (auto& sampPureWeak : vec)
                                {
                                    auto sampPure = sampPureWeak.lock();
                                    if (sampPure)
                                    {
                                        auto samp = std::static_pointer_cast<VkSampler>(sampPure);
                                        samplerInfos.emplace_back(CreateSamplerInfo(samp));
                                    } else samplerInfos.push_back(dummySamplerInfo);
                                }
                            }
                        }
                    }, bindIt->second.resources);
                }

                for (std::size_t i = existing_n, n = binding.count; i < n; ++i)
                    samplerInfos.push_back(dummySamplerInfo);

                write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                write.pImageInfo = samplerInfos.data() + (samplerInfos.size() - binding.count);
                break;
            }
            case BindingType::Unknown:
            {
                HL_EXCEPTION(true, "Unknown binding type");
                break;
            }
            }
            writes.push_back(write);
        }

        // HL_DEBUGBREAK();
        vkUpdateDescriptorSets(*device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

    // OLD IN 1 FUNCTION VERSION
    // void WriteBindings(const Ref<VkGraphicsDevice>& device, VkDescriptorSet descriptorSet, const BindingLayout& bindingLayout, const ResourceBindingMap& bindingMap)
    // {
    //     std::vector<VkWriteDescriptorSet> writes;
    //     writes.reserve(bindingLayout.m_bindings.size());
    //     std::vector<VkDescriptorBufferInfo> bufferInfos;
    //     std::vector<VkDescriptorImageInfo> imageInfos;

    //     for (std::size_t i = 0, n = bindingLayout.m_bindings.size(); i < n; ++i)
    //     {
    //         auto& binding = bindingLayout.m_bindings[i];

    //         auto typeIt = bindingMap.bindings.find(binding.type);

    //         VkWriteDescriptorSet write{};
    //         write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //         write.dstSet = descriptorSet;
    //         write.dstBinding = binding.slot;
    //         write.dstArrayElement = 0;
    //         write.descriptorCount = binding.count;

    //         bool typeExists = typeIt != bindingMap.bindings.end();

    //         switch (binding.type) {
    //         case BindingType::UniformBuffer:
    //         {
    //             bufferInfos.reserve(bufferInfos.size() + binding.count);

    //             std::size_t existing_n = 0;
    //             auto bindIt = typeExists ? typeIt->second.find(binding.slot) : typeIt->second.end();
    //             if (bindIt != typeIt->second.end())
    //             {
    //                 std::visit([&](auto&& vec){
    //                     using T = std::decay_t<decltype(vec)>;
    //                     if constexpr (std::is_same_v<T, std::vector<WeakRef<UnsafeUniformBuffer>>>)
    //                     {
    //                         existing_n = std::min(static_cast<std::uint32_t>(vec.size()), binding.count);
    //                         if (existing_n > 0)
    //                         {
    //                             for (auto& ubPureWeak : vec)
    //                             {
    //                                 auto ubPure = ubPureWeak.lock();
    //                                 auto ub = std::static_pointer_cast<VkUniformBuffer>(ubPure);
    //                                 VkDescriptorBufferInfo bufferInfo{};
    //                                 bufferInfo.buffer = ub->GetBuffer();
    //                                 bufferInfo.offset = 0;
    //                                 bufferInfo.range = ub->GetSize();
    //                                 bufferInfos.push_back(bufferInfo);
    //                             }
    //                         }
    //                     }
    //                 }, bindIt->second.resources);
    //             }

    //             if (existing_n != binding.count)
    //             {
    //                 auto ub = std::static_pointer_cast<VkUniformBuffer>(device->GetDummyUniformBuffer());
    //                 assert(ub);
    //                 for (std::size_t i = existing_n, n = binding.count; i < n; ++i)
    //                 {
    //                     VkDescriptorBufferInfo bufferInfo{};
    //                     bufferInfo.buffer = ub->GetBuffer();
    //                     bufferInfo.offset = 0;
    //                     bufferInfo.range = ub->GetSize();
    //                     bufferInfos.push_back(bufferInfo);
    //                 }
    //             }

    //             write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //             write.pBufferInfo = bufferInfos.data() + (bufferInfos.size() - binding.count);
    //             break;
    //         }
    //         case BindingType::Texture2D:
    //         {
    //             imageInfos.reserve(imageInfos.size() + binding.count);

    //             std::size_t existing_n = 0;
    //             auto bindIt = typeExists ? typeIt->second.find(binding.slot) : typeIt->second.end();
    //             if (bindIt != typeIt->second.end())
    //             {
    //                 std::visit([&](auto&& vec){
    //                     using T = std::decay_t<decltype(vec)>;
    //                     if constexpr (std::is_same_v<T, std::vector<WeakRef<Texture2D>>>)
    //                     {
    //                         existing_n = std::min(static_cast<std::uint32_t>(vec.size()), binding.count);
    //                         if (existing_n > 0)
    //                         {
    //                             for (auto& texPureWeak : vec)
    //                             {
    //                                 auto texPure = texPureWeak.lock();
    //                                 auto tex = std::static_pointer_cast<VkTexture2D>(texPure);
    //                                 VkDescriptorImageInfo imageInfo{};
    //                                 imageInfo.imageView = tex->GetImageView();
    //                                 imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //                                 imageInfos.push_back(imageInfo);
    //                             }
    //                         }
    //                     }
    //                 }, bindIt->second.resources);
    //             }

    //             if (existing_n != binding.count)
    //             {
    //                 auto tex = std::static_pointer_cast<VkTexture2D>(device->GetDummyTexture2D());
    //                 assert(tex);
    //                 for (std::size_t i = existing_n, n = binding.count; i < n; ++i)
    //                 {
    //                     VkDescriptorImageInfo imageInfo{};
    //                     imageInfo.imageView = tex->GetImageView();
    //                     imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //                     imageInfos.push_back(imageInfo);
    //                 }
    //             }

    //             write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    //             write.pImageInfo = imageInfos.data() + (imageInfos.size() - binding.count);
    //             break;
    //         }
    //         case BindingType::Sampler:
    //         {
    //             imageInfos.reserve(imageInfos.size() + binding.count);

    //             std::size_t existing_n = 0;
    //             auto bindIt = typeExists ? typeIt->second.find(binding.slot) : typeIt->second.end();
    //             if (bindIt != typeIt->second.end())
    //             {
    //                 std::visit([&](auto&& vec){
    //                     using T = std::decay_t<decltype(vec)>;
    //                     if constexpr (std::is_same_v<T, std::vector<WeakRef<Sampler>>>)
    //                     {
    //                         existing_n = std::min(static_cast<std::uint32_t>(vec.size()), binding.count);
    //                         if (existing_n > 0)
    //                         {
    //                             for (auto& sampPureWeak : vec)
    //                             {
    //                                 auto sampPure = sampPureWeak.lock();
    //                                 auto samp = std::static_pointer_cast<VkSampler>(sampPure);
    //                                 VkDescriptorImageInfo samplerInfo{};
    //                                 samplerInfo.sampler = samp->GetNative();
    //                                 imageInfos.push_back(samplerInfo);
    //                             }
    //                         }
    //                     }
    //                 }, bindIt->second.resources);
    //             }

    //             if (existing_n != binding.count)
    //             {
    //                 auto samp = std::static_pointer_cast<VkSampler>(device->GetDummySampler());
    //                 assert(samp);
    //                 for (std::size_t i = existing_n, n = binding.count; i < n; ++i)
    //                 {
    //                     VkDescriptorImageInfo samplerInfo{};
    //                     samplerInfo.sampler = samp->GetNative();
    //                     imageInfos.push_back(samplerInfo);
    //                 }
    //             }

    //             write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    //             write.pImageInfo = imageInfos.data() + (imageInfos.size() - binding.count);
    //             break;
    //         }
    //         }
    //         writes.push_back(write);
    //     }

    //     vkUpdateDescriptorSets(*device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    // }

    // Ref<VkDescriptorSet> VkDescriptorSetCache::GetDescriptorSet(const VkDescriptorSetKey &key, VkDescriptorSetLayout &vklayout, bool transient)
    // {
    //     auto &cache =
    //         transient ? m_transientDescriptorSetCache : m_staticDescriptorSetCache;
    //     auto it = cache.find(key);
    //     if (it != cache.end()) {
    //         it->second.lastUsage = Time::CurrentTimeMicroseconds();
    //         return it->second.descriptorSet;
    //     }

    //     VkDescriptorSet descriptorSet = AllocateDescriptorSet(m_device->GetNativeDevice(),
    //         transient ? m_transientDescriptorPools[Graphics::GetCurrentFrameIndex()]
    //                     : m_staticDescriptorPool,
    //         vklayout);

    //     WriteBindings(m_device->GetNativeDevice(), descriptorSet, key.layout, key.bindingMap);

    //     Ref<VkDescriptorSet> sharedDescSet =
    //         CreateRef<VkDescriptorSet>(descriptorSet);
    //     cache[key] = {sharedDescSet, Graphics::GetCurrentFrameIndex()};

    //     return sharedDescSet;
    // }

    Ref<VkDescriptorSet> VkDescriptorSetCache::Acquire(const VkDescriptorSetKey& key, VkDescriptorSetLayout& vklayout)
    {
        HL_EXCEPTION(key.layout.m_bindings.empty(), "Cannot acquire an VkDescriptorSet of empty layout!");
        auto &cache = m_transientDescriptorSetCache;
        auto it = cache.find(key);
        if (it != cache.end()) {
            std::cout << "DescriptorSet CACHE HIT - reusing set " << it->second.second.descriptorSet.get() << std::endl;
            it->second.second.lastUsage = Time::CurrentTimeMicroseconds();
            it->second.first++;
            return it->second.second.descriptorSet;
        }

        std::cout << "DescriptorSet CACHE MISS - creating new set" << std::endl;
        
        VkDescriptorSet descriptorSet = AllocateDescriptorSet(
            m_device->GetNativeDevice(),
            m_transientDescriptorPools[Graphics::GetCurrentFrameIndex()],
            vklayout
        );

        WriteBindings(m_device, descriptorSet, key.layout, key.bindingMap);

        Ref<VkDescriptorSet> sharedDescSet =
            CreateRef<VkDescriptorSet>(descriptorSet);
        cache[key] = { 1u, {sharedDescSet, Time::CurrentTimeMicroseconds()} };

        std::cout << "DescriptorSet created: " << descriptorSet << std::endl;

        return sharedDescSet;
    }

    void VkDescriptorSetCache::Prepare()
    {
        std::vector<VkDescriptorSetKey> toRemove;
        for (auto &[key, item] : m_staticDescriptorSetCache) {
            if (item.descriptorSet.use_count() <= 1) {
            vkFreeDescriptorSets(m_device->GetNativeDevice(), m_staticDescriptorPool, 1,
                                item.descriptorSet.get());
            toRemove.push_back(key);
            }
        }
        for (const auto &key : toRemove)
            m_staticDescriptorSetCache.erase(key);

        m_device->GetSyncPool().Wait(m_tickets[Graphics::GetCurrentFrameIndex()]);


        auto currentTransientPool = m_transientDescriptorPools[Graphics::GetCurrentFrameIndex()];

        {
            std::lock_guard<std::mutex> lock(m_device->m_graphicsQueueMutex);
            vkDeviceWaitIdle(m_device->GetNativeDevice());
        }

        vkResetDescriptorPool(m_device->GetNativeDevice(), currentTransientPool, 0);

        m_transientDescriptorSetCache.clear();
    }
} // namespace Helios